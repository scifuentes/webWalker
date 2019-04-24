# walkSchemas generation for webServos


def title(name):
    print
    print name,':'
def clear():
    print 'mv.clear '
def go():
    print 'mv.go '
def mv(pos,spd=5):
    print 'mv.add',spd,' '.join(str(p) for p in pos)
def set(pos):
    print 'servos.set',' '.join(str(p) for p in pos)
def add(l0,l1):
    return [v0+v1 for v0,v1 in zip(l0,l1)]

#[front_left front_right back_left back_right]
#[z x]
zero = [90]*8
base = [90 ,90, 90, 90, 90, 90, 90, 90]
up_abs = [50, 90, 130, 90, 130, 90, 50, 90] #body up, all legs down
fwd_abs= [90, 130, 90, 50, 90, 130, 90, 50] #body fwd, all legs back
fwd_rel = [0,40,0,-40,0,40,0,-40]
up_rel = [-40,0,40,0,40,0,-40,0]
up1 =  [-1,0,1, 0,1,0,-1,0]
fwd1 = [ 0,1,0,-1,0,1, 0,-1]
rot1 = [ 0,1,0, 1,0,1, 0, 1]


def bodyMove(x=0,z=0,r=0,ref=base):
    mv_rel = add(add([z*v for v in up1],[x*v for v in fwd1]),[r*v for v in rot1])
    return add(mv_rel,ref)

def legMove(ileg,x=0,z=0,ref=base):
    ileg_z=2*ileg
    ileg_x=2*ileg+1
    mv_rel = [0]*8
    mv_rel[ileg_z] = -z*up1[ileg_z]
    mv_rel[ileg_x] = -x*fwd1[ileg_x]
    return add(mv_rel,ref)

class Robot:
    def __init__(self,ref=base):
        self.pos=ref
    def bodyMove(self,x=0,z=0,r=0):
        self.pos = bodyMove(x,z,r,self.pos)
        return self.pos
    def legMove(self,ileg,x=0,z=0):
        self.pos = legMove(ileg,x,z,self.pos)
        return self.pos
    def set(self,pos):
        self.pos=pos
        return self.pos
    def get(self,):
        return self.pos

up = bodyMove(z=40)
down = bodyMove(z=-40)
fwd = bodyMove(x=40)
back = bodyMove(x=-40)

title('Zero')
set(zero)

title('Base')
set(base)

title('Up')
set(up)

title('Fwd')
set(fwd)

title('LeftRight')
clear()
mv(bodyMove(r=40))
mv(bodyMove(r=-40))
mv(base)
go()

title('Warm up')
clear()
set(base)

mv(fwd)
mv(back)
mv(base)

mv(up)
mv(down)
mv(base)

mv(bodyMove(r=20))
mv(bodyMove(r=-20))
mv(base)

r=Robot(base)
mv(r.bodyMove(x=-40)) #back
mv(r.legMove(0,z=40))
mv(r.legMove(0,z=-40))
mv(r.legMove(1,z=40))
mv(r.legMove(1,z=-40))
mv(r.bodyMove(x=+80)) #fwd
mv(r.legMove(2,z=40))
mv(r.legMove(2,z=-40))
mv(r.legMove(3,z=40))
mv(r.legMove(3,z=-40))
mv(r.set(base)) #base
go()


