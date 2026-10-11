"""Differential and edge-case tests for Task 1 (run from the repository root: make test)."""
import subprocess, random, re
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent
BIN = str(ROOT / "build" / "task1_temperature_monitoring")
def run(inp):
    r=subprocess.run([BIN],input=inp,capture_output=True,text=True,timeout=10)
    assert r.returncode==0,(r.returncode,r.stderr)
    assert 'runtime error' not in r.stderr and 'ERROR' not in r.stderr, r.stderr
    o=r.stdout.replace('Your Choice: ','Your Choice: \n').replace('Temperature In Question: ','Temperature In Question: \n')
    return re.sub(r'(separated by a space\): )',r'\1\n',o)
random.seed(7)
def manual(data):
    return f'1\n{len(data)}\n'+''.join(f'{x}\n' for x in data)
cases=0
for trial in range(300):
    n=random.randint(1,40)
    lo=random.randint(-30,10); hi=lo+random.randint(0,25)
    data=sorted(random.randint(lo,hi) for _ in range(n))
    inp=manual(data)
    queries=[]
    for _ in range(6):
        k=random.randint(lo-3,hi+3)
        queries.append(('3',k)); inp+=f'3\n{k}\n'
    for _ in range(6):
        a=random.randint(lo-5,hi+5); b=a+random.randint(0,15)
        queries.append(('4',(a,b))); inp+=f'4\n{a} {b}\n'
    inp+='0\n'
    out=run(inp)
    # split outputs per query: take lines after menu
    lines=[l for l in out.split('\n') if l.startswith(('Temperature','No Temperatures')) and not l.startswith(('Temperature In Question','Temperature Range'))]
    it=iter(lines)
    for kind,q in queries:
        if kind=='3':
            l=next(it)
            if q not in data:
                assert l=='Temperature Not Found.',(data,q,l)
            else:
                f=data.index(q); la=len(data)-1-data[::-1].index(q)
                if f==la: assert l==f'Temperature Found On Position: {f}',(data,q,l)
                else:
                    assert l==f'Temperature On First Position: {f}',(data,q,l)
                    l2=next(it); assert l2==f'Temperature On Last Position: {la}',(data,q,l2)
        else:
            l=next(it); a,b=q
            exp=[(x,i) for i,x in enumerate(data) if a<=x<=b]
            if not exp: assert l=='No Temperatures In Range.',(data,q,l)
            else:
                assert l=='Temperatures In Range (Temperature, Position): '+', '.join(f'({x}, {i})' for x,i in exp),(data,q,l)
            cases+=1
        cases+=1
    assert next(it,None) is None
print('random manual-input tests passed, queries checked:',cases)

# validation: unsorted rejected & re-prompted, bad numbers, >365, bad range
out=run('1\n3\n5\n2\n7\nabc\n9\n3\n2\n3\n7\n3\n9\n0\n')
assert 'Non-Decreasing' in out and 'Invalid Input' in out
assert 'Temperature Not Found.' in out and 'Temperature Found On Position: 1' in out and 'Temperature Found On Position: 2' in out
print('unsorted input is rejected and re-prompted; data kept = [5,7,9]')
assert 'Invalid Input' in run('1\n366\n0\n') and 'Invalid Input' in run('1\n0\n0\n')
assert 'Invalid Input' in run('2\n10\n5 1\n0\n') and 'Invalid Input' in run('2\n10\n5\n0\n')
assert 'No Data Loaded.' in run('3\n0\n') and 'No Data Loaded.' in run('4\n0\n')
# re-entering after clear works (old bug)
out=run(manual([1,2,2,3])+'5\n'+manual([10,20])+'3\n20\n3\n2\n0\n')
assert 'Data Was Successfully Cleared.' in out and 'Temperature Found On Position: 1' in out and out.count('Temperature Not Found.')==1
print('clear + re-entering data works')
# generation: 365 values in range, sorted, repeated allowed
for cnt,a,b in [(365,-20,40),(50,5,5),(1,0,0),(100,-10,-5)]:
    out=run(f'2\n{cnt}\n{a} {b}\n6\n0\n')
    nums=[int(t) for l in out.split('\n') if re.fullmatch(r'-?\d+( -?\d+)*',l.strip()) for t in l.split()]
    assert len(nums)==cnt and nums==sorted(nums) and all(a<=x<=b for x in nums),(cnt,a,b)
print('generation OK (count, range, sorted, duplicates allowed)')
# EOF handling
run('3\n'); run('1\n'); run('1\n3\n5\n'); run(''); run('abc\n0\n')
print('EOF + garbage menu input handled')
# 365 boundary
run(manual(list(range(365)))+'3\n364\n0\n')
print('ALL TASK 1 TESTS PASSED')
