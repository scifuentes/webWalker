# walkSchemas generation for webServos


def title(name):
    print name,':'
def clear():
    print 'mv.clear'
def go():
    print 'mv.go'
def mv(pos,spd=10):
    print 'mv.add',spd,' '.join(str(p) for p in pos)
def set(pos):
    print 'servos.set',' '.join(str(p) for p in pos)
def add(l0,l1):
    return [v0+v1 for v0,v1 in zip(l0,l1)]
def diff(l0,l1):
    return [v0-v1 for v0,v1 in zip(l0,l1)]

#[front_left front_right back_left back_right]
#[z x]
zero = [90]*8
base = [90,90, 90, 90, 90, 90, 90, 90]
up_abs = [50, 90, 130, 90, 130, 90, 50, 90] #body up, all legs down
fwd_abs= [90, 130, 90, 50, 90, 130, 90, 50] #body fwd, all legs back

up_rel = [-40,0,40,0,40,0,-40,0]
up = add(base,up_rel)
down = diff(base,up_rel)
fwd_rel = [0,40,0,-40,0,40,0,-40]
fwd = add(base,fwd_rel)
back = diff(base,fwd_rel)

title('Zero')
set(zero)

title('Base')
set(base)

title('Up')
set(up)

title('Fwd')
set(fwd)

title('Warm up')
clear()
set(base)
mv(fwd)
mv(back)
mv(base)
mv(up)
mv(down)
mv(base)
go()


