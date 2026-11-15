"""Correctness model, not a production implementation or performance benchmark.
The oracle recomputes node sets explicitly. Packed static WM and signed Patricia
cursors implement the proposed baseline/delta queries independently.
"""
import heapq
import random

class Wavelet:
    def __init__(self, a, height):
        self.height = height
        self.levels = []
        for bit in range(height-1, -1, -1):
            words = [0]*((len(a)+63)//64)
            for i,x in enumerate(a):
                words[i//64] |= ((x>>bit)&1) << (i%64)
            pref = [0]
            for word in words: pref.append(pref[-1]+word.bit_count())
            zeros = len(a)-pref[-1]
            self.levels.append((words,pref,zeros))
            a = [x for x in a if not (x>>bit)&1] + [x for x in a if (x>>bit)&1]
    def rank1(self, level, p):
        words,pref,_ = self.levels[level]
        return pref[p//64] + ((words[p//64]&((1<<(p%64))-1)).bit_count() if p%64 else 0)
    def split(self, level, l, r):
        lo,hi = self.rank1(level,l),self.rank1(level,r)
        zeros = self.levels[level][2]
        return (l-lo,r-hi),(zeros+lo,zeros+hi)

class Trie:
    def __init__(self, key, weight=0, bit=-1, children=None):
        self.key,self.weight,self.bit,self.children = key,weight,bit,children
        self.total = weight if bit<0 else sum(x.total for x in children)

def add(t,key,delta):
    if not delta: return t
    if t is None: return Trie(key,delta)
    if t.bit<0 and t.key==key:
        weight=t.weight+delta
        return Trie(key,weight) if weight else None
    bit=(key^t.key).bit_length()-1
    if bit>t.bit:
        children=[None,None]
        side=(key>>bit)&1
        children[side]=Trie(key,delta)
        children[side^1]=t
        return Trie(children[0].key,bit=bit,children=children)
    side=(key>>t.bit)&1
    t.children[side]=add(t.children[side],key,delta)
    if t.children[side] is None: return t.children[side^1]
    t.key=t.children[0].key
    t.total=t.children[0].total+t.children[1].total
    # A total of zero does NOT mean that this subtree is empty.
    return t

def split_trie(t,bit):
    if t is None: return None,None
    if t.bit==bit: return t.children
    assert t.bit<bit
    return (None,t) if (t.key>>bit)&1 else (t,None)

def total(t): return t.total if t else 0

class Epoch:
    def __init__(self, values, queries, raw=False):
        self.n=len(values)
        self.base=1<<(max(1,self.n)-1).bit_length()
        all_values=values+[q[2] for q in queries]
        if raw:
            # Raw integer keys need only the epoch capacity, not future values
            # for sorting. Future keys could be created when each query arrives.
            id_bits=max(1,(len(all_values)-1).bit_length())
            mask=(1<<64)-1
            ranks=[((~((x&mask)^(1<<63))&mask)<<id_bits)|i for i,x in enumerate(all_values)]
            self.height=64+id_bits
        else:
            ids=sorted(range(len(all_values)),key=lambda i:(-all_values[i],i))
            ranks=[0]*len(ids)
            for rank,i in enumerate(ids): ranks[i]=rank
            self.height=max(1,(len(ids)-1).bit_length())
        self.value=dict(zip(ranks,all_values))
        self.inputs=ranks[self.n:]
        self.a0=ranks[:self.n]
        self.wm=Wavelet(self.a0,self.height)
        self.position={key:i for i,key in enumerate(self.a0)}
        self.bounds=[None]*(2*self.base)
        def bounds(v,l,length):
            self.bounds[v]=(min(l,self.n),min(l+length,self.n),length)
            if length>1:
                bounds(2*v,l,length//2)
                bounds(2*v+1,l+length//2,length//2)
        bounds(1,0,self.base)
        self.actual=self.recompute()
        self.roots=[[None]*4 for _ in self.actual]
        self.zero_sum_nonempty=0
        self.max_member_edits=0
        for v in range(1,len(self.actual)):
            assert not self.actual[v][0] and not self.actual[v][3]
            assert self.actual[v][1]==set(self.a0[slice(*self.bounds[v][:2])])
            if v<self.base:
                assert self.actual[v][2]==set(self.a0[slice(*self.bounds[2*v+1][:2])])

    def recompute(self):
        sets=[[set() for _ in range(4)] for _ in self.bounds]
        assigned=[[] for _ in range(self.n)]
        for key,p in self.position.items(): assigned[p].append(key)
        for p,keys in enumerate(assigned):
            if keys:
                d=min(keys)
                sets[self.base+p][0]=set(keys)-{d}
                sets[self.base+p][1]={d}
        for v in range(self.base-1,0,-1):
            nl,dl,_,_=sets[2*v]
            nr,dr,_,_=sets[2*v+1]
            w=sorted(nl|dr)
            k=min(self.bounds[v][2]//2,len(w))
            low,high=set(w[:k]),set(w[k:])
            sets[v]=[nr|high,dl|low,low,high]
        return sets

    def base_interval(self,v,which):
        if which==1: return self.bounds[v][:2]
        if which==2 and v<self.base: return self.bounds[2*v+1][:2]
        return 0,0

    def edit(self,key,p,adding):
        if adding: self.position[key]=p
        else: del self.position[key]
        fresh=self.recompute()
        ancestors=set()
        v=self.base+p
        while v:
            ancestors.add(v)
            v//=2
        for v in range(1,len(fresh)):
            if v not in ancestors:
                assert fresh[v]==self.actual[v]
                continue
            changes=0
            for which in range(4):
                old,new=self.actual[v][which],fresh[v][which]
                for x in old-new: self.roots[v][which]=add(self.roots[v][which],x,-1)
                for x in new-old: self.roots[v][which]=add(self.roots[v][which],x,1)
                changes+=len(old^new)
                l,r=self.base_interval(v,which)
                assert r-l+total(self.roots[v][which])==len(new)
                self.zero_sum_nonempty+=int(self.roots[v][which] is not None and total(self.roots[v][which])==0)
            assert changes<=8, changes
            self.max_member_edits=max(self.max_member_edits,changes)
        self.actual=fresh

    def select(self,v,which,k):
        l,r=self.base_interval(v,which)
        t=self.roots[v][which]
        assert 0<=k<r-l+total(t)
        result=0
        for level,bit in enumerate(range(self.height-1,-1,-1)):
            ranges=self.wm.split(level,l,r)
            children=split_trie(t,bit)
            left_count=ranges[0][1]-ranges[0][0]+total(children[0])
            assert left_count>=0
            side=int(k>=left_count)
            if side: k-=left_count
            result=(result<<1)|side
            l,r=ranges[side]
            t=children[side]
        return result

    def range_min_id(self,l,r):
        assert l<r
        parts=[]
        def cover(v,begin,end,ql,qr):
            if qr<=begin or end<=ql: return
            if ql<=begin and end<=qr:
                dl,dr=self.base_interval(v,1)
                parts.append([dl,dr,self.roots[v][0],self.roots[v][1],0,0,end-begin])
                return
            mid=(begin+end)//2
            cover(2*v,begin,mid,ql,qr)
            cover(2*v+1,mid,end,ql,qr)
        cover(1,0,self.base,0,l)
        prefix=len(parts)
        cover(1,0,self.base,l,r)
        result=0
        for level,bit in enumerate(range(self.height-1,-1,-1)):
            splits=[]
            carry=0
            ok=True
            for i,(dl,dr,nt,dt,nb,db,length) in enumerate(parts):
                ranges=self.wm.split(level,dl,dr)
                ns,ds=split_trie(nt,bit),split_trie(dt,bit)
                nc=total(ns[0])
                dc=ranges[0][1]-ranges[0][0]+total(ds[0])
                assert nc>=0 and dc>=0
                small_n,small_d=nb+nc,db+dc
                if i>=prefix and carry+small_d<length: ok=False
                carry=small_n+max(0,carry+small_d-length)
                splits.append((ranges,ns,ds,nc,dc))
            side=int(not ok)
            result=(result<<1)|side
            for part,(ranges,ns,ds,nc,dc) in zip(parts,splits):
                part[0],part[1]=ranges[side]
                part[2],part[3]=ns[side],ds[side]
                if side:
                    part[4]+=nc
                    part[5]+=dc
        return result

    def output_ids(self):
        assigned=[[] for _ in range(self.n)]
        for key,p in self.position.items(): assigned[p].append(key)
        heap,result=[],[]
        for keys in assigned:
            for key in keys: heapq.heappush(heap,key)
            result.append(heapq.heappop(heap))
        assert not heap
        return result


def main():
    if not __debug__:
        raise RuntimeError("Run without -O: this correctness model relies on assertions.")
    # The root is nonempty even though +1 and -1 cancel in its total.
    t=add(add(None,0,-1),3,1)
    assert t is not None and t.total==0 and t.children is not None
    rng=random.Random(12026)
    checked=epochs=zero_sum=maximum=0
    for n in (0,1,2,3,7,8,17,31,63,64,65,127):
        for mode in range(3):
            for block in sorted({1,max(1,n//max(1,n.bit_length())),n+3}):
                a=[rng.randrange(-5,6) for _ in range(n)]
                if mode==2: a=[rng.getrandbits(64)-(1<<63) for _ in range(n)]
                done=0
                while done<120:
                    queries=[]
                    for q in range(min(block,120-done)):
                        l,r=sorted((rng.randrange(n+1),rng.randrange(n+1)))
                        if mode==1 and done<2*n: l,r=0,n
                        x=done+q+20 if mode==1 else rng.randrange(-10,11)
                        if mode==2: x=rng.getrandbits(64)-(1<<63)
                        queries.append((l,r,x))
                    e=Epoch(a,queries)
                    for j,(l,r,x) in enumerate(queries):
                        ids=e.output_ids()
                        if l<r:
                            key=e.range_min_id(l,r)
                            assert key==max(ids[l:r])
                            answer=e.value[key]
                            if x>answer:
                                e.edit(key,e.position[key],False)
                                e.edit(e.inputs[j],l,True)
                            else: answer=x
                        else: answer=x
                        want=x
                        for i in range(l,r):
                            if a[i]<want: a[i],want=want,a[i]
                        assert answer==want,(n,mode,block,done,j)
                        assert [e.value[i] for i in e.output_ids()]==a
                        if j%7==0:
                            for v in range(1,len(e.actual)):
                                for which,s in enumerate(e.actual[v]):
                                    if s:
                                        assert e.select(v,which,0)==min(s)
                                        assert e.select(v,which,len(s)-1)==max(s)
                        checked+=1
                    zero_sum+=e.zero_sum_nonempty
                    maximum=max(maximum,e.max_member_edits)
                    epochs+=1
                    done+=len(queries)
    raw_checked=0
    for n in (1,7,65):
        a=[rng.getrandbits(64)-(1<<63) for _ in range(n)]
        for epoch in range(4):
            queries=[]
            for q in range(20):
                l,r=sorted((rng.randrange(n+1),rng.randrange(n+1)))
                x=rng.getrandbits(64)-(1<<63)
                if q==0: x=-(1<<63)
                if q==1: x=(1<<63)-1
                queries.append((l,r,x))
            e=Epoch(a,queries,raw=True)
            for j,(l,r,x) in enumerate(queries):
                if l<r:
                    key=e.range_min_id(l,r)
                    assert key==max(e.output_ids()[l:r])
                    answer=e.value[key]
                    if x>answer:
                        e.edit(key,e.position[key],False)
                        e.edit(e.inputs[j],l,True)
                    else: answer=x
                else: answer=x
                want=x
                for i in range(l,r):
                    if a[i]<want: a[i],want=want,a[i]
                assert answer==want
                assert [e.value[i] for i in e.output_ids()]==a
                raw_checked+=1
    print(f'Raw 64-bit value + epoch-ID keys: {raw_checked} additional queries matched.')
    print(f'OK: {checked} queries, {epochs} epochs; answers, arrays, synchronous range minima and set extrema matched.')
    print(f'Nonempty zero-sum delta roots observed: {zero_sum}; maximum total member edits at one ancestor for one history edit: {maximum}.')

if __name__=='__main__': main()
