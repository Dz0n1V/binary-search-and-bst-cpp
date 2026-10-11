"""Validity and edge-case tests for Task 2 (run from the repository root: make test)."""
import subprocess, re, math, random, csv, sys, tempfile, os
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent
BIN = str(ROOT / "build" / "task2_hotel_loyalty_bst")
DATA = ROOT / "data"
TMP = tempfile.mkdtemp()
def run(inp):
    r=subprocess.run([BIN],input=inp,capture_output=True,text=True,timeout=10)
    assert r.returncode==0,(r.returncode,r.stderr)
    assert 'ERROR' not in r.stderr and 'runtime error' not in r.stderr, r.stderr
    return r.stdout.replace('Choose An Option: ','Choose An Option: \n')
def load(fn):
    rows=list(csv.reader(open(fn)))[1:]
    return [(int(p),f,l) for p,f,l in rows]
def parse_tree(out):
    lines=[l for l in out.split('\n') if l.startswith('Parent:')]
    pat=re.compile(r'Parent: (.*?) \((-?\d+)\) Left Child: (?:None|(.*?) \((-?\d+)\)) Right Child: (?:None|(.*?) \((-?\d+)\))$')
    nodes={}
    for l in lines:
        m=pat.match(l); assert m,l
        g=m.groups()
        nodes[g[0]]=(int(g[1]),g[2] if g[2] else None,g[4] if g[4] else None)
    root=pat.match(lines[0]).group(1)
    return nodes,root
def check_bst(nodes,root,expected_n):
    seen=[]
    def rec(k,lo,hi):
        if k is None: return 0
        p,l,r=nodes[k]; seen.append(p)
        assert (lo is None or p>=lo) and (hi is None or p<=hi),(k,p,lo,hi)
        return 1+max(rec(l,lo,p),rec(r,p,hi))
    h=rec(root,None,None)
    assert len(seen)==expected_n==len(nodes)
    return h
for fn in [str(DATA/'gosti_poeni_50.csv'),str(DATA/'gosti_poeni_100.csv')]:
    g=load(fn); n=len(g)
    print(f'--- {fn}: {n} guests')
    out=run(f'1\n{fn}\n5\n0\n')
    nodes,root=parse_tree(out)
    h=check_bst(nodes,root,n)
    print(' loaded, BST valid, height',h,'minimal',math.ceil(math.log2(n+1)))
    assert h==math.ceil(math.log2(n+1))
    # search by points: every guest
    inp='1\n%s\n'%fn+''.join(f'2\n{p}\n' for p,_,_ in g)+'2\n999999\n0\n'
    out=run(inp)
    for p,f,l in g: assert f'Guest Found: {f} {l} - {p} Points.' in out
    assert out.count('Guest Not Found.')==1
    print(' search by points OK for all',n)
    # inorder sorted
    out=run(f'1\n{fn}\n4\n0\n')
    ino=[int(x.split(' - ')[0]) for x in out.split('\n') if re.match(r'^-?\d+ - ',x)]
    assert ino==sorted(p for p,_,_ in g); print(' inorder sorted OK')
    # closest higher vs brute force
    s=sorted(g)
    inp='1\n%s\n'%fn+''.join(f'7\n{f}\n{l}\n' for _,f,l in s)+'0\n'
    out=run(inp)
    res=re.findall(r'Guest With Closest Higher Points:\n(\d+) - (.+)\n|(No Guest Has Higher Points)\.',out)
    assert len(res)==n
    for i,(p,f,l) in enumerate(s):
        if i==n-1: assert res[i][2]=='No Guest Has Higher Points',res[i]
        else: assert (res[i][0],res[i][1])==(str(s[i+1][0]),s[i+1][1]+' '+s[i+1][2]),(i,res[i])
    print(' closest-higher OK for all',n,'(incl. largest -> none)')
    # random increments, validate after each
    random.seed(1)
    cur={(f,l):p for p,f,l in g}
    steps=[]
    for _ in range(40):
        f,l=random.choice(list(cur)); inc=random.choice([0,1,50,-30,500,1000]); cur[(f,l)]+=inc; steps.append((f,l,inc))
    inp='1\n%s\n'%fn+''.join(f'3\n{f}\n{l}\n{inc}\n' for f,l,inc in steps)+'5\n0\n'
    out=run(inp); nodes,root=parse_tree(out)
    check_bst(nodes,root,n)
    assert {k:v[0] for k,v in nodes.items()}=={f+' '+l:p for (f,l),p in cur.items()}
    print(' 40 random point increases: BST valid, all values correct')
    # delete tree then operations
    out=run(f'1\n{fn}\n6\n4\n5\n2\n10\n7\nA\nB\n0\n')
    assert out.count('Tree Is Empty.')==2 and 'Guest Not Found.' in out
    print(' delete tree + ops on empty tree OK')
# error paths
open(os.path.join(TMP,'bad_format.csv'),'w').write('p,i,pr\n10,A,B\nxx,C,D\n')
open(os.path.join(TMP,'dup.csv'),'w').write('p,i,pr\n10,A,B\n10,A,B\n')
open(os.path.join(TMP,'crlf.csv'),'w',newline='').write('p,i,pr\r\n10,Ana,Anic\r\n20,Bojan,Bojic\r\n30,Cara,Caric\r\n')
open(os.path.join(TMP,'empty.csv'),'w').write('p,i,pr\n')
interleaved='p,i,pr\n10,A,B\n10,C,D\n10,A,B\n'
open(os.path.join(TMP,'dup2.csv'),'w').write(interleaved)
for f,exp in [(os.path.join(TMP,'bad_format.csv'),'Wrong Format At Line 3'),(os.path.join(TMP,'dup.csv'),'Duplicate Entry Found'),(os.path.join(TMP,'empty.csv'),'File Is Empty'),(os.path.join(TMP,'nofile.csv'),'Cannot Open File'),(os.path.join(TMP,'dup2.csv'),'Duplicate Entry Found')]:
    out=run(f'1\n{f}\n0\n'); assert exp in out and 'Loading Failed.' in out,(f,out); print(' error path OK:',f)
out=run('1\n'+os.path.join(TMP,'crlf.csv')+'\n2\n20\n3\nAna\nAnic\n15\n4\n0\n'); assert 'Guest Found: Bojan Bojic - 20' in out and '45 - ' not in out and '25 - Ana Anic' in out
print(' CRLF file OK')
out=run('abc\n\n9\n3\nA\nB\nx\n0\n'); assert out.count('Invalid')>=2
run('4\n'); run('1\n'); run('')  # EOF must terminate
print(' invalid input + EOF termination OK')
print('ALL TASK 2 TESTS PASSED')
