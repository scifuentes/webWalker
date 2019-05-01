# walkSchemas generation for webServos
import urllib, urllib2

def title(name):
    print
    print name,':'

def clear():
    return 'mv.clear '
def go():
    return 'mv.go '
def once():
    return 'mv.once '
def loop():
    return 'mv.loop '
def mv(pos,spd=5):
    return 'mv.add '+str(spd)+' '+' '.join(str(p) for p in pos)
def set(pos):
    return 'servos.set '+' '.join(str(p) for p in pos)

def send(cmd):
    data = urllib.urlencode({"Commands_text":cmd})
    req = urllib2.Request('http://192.168.1.83:83/commands', data)
    response = urllib2.urlopen(req)

def add(l0,l1):
    return [v0+v1 for v0,v1 in zip(l0,l1)]
def scale(vector,factor):
    return [v*factor for v in vector]
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


def bodyPos(pos=None,x=0,z=0,r=0,ref=base):
    if pos:
        pos_rel = [p*-(h+v) for p,h,v in zip(pos,fwd1,up1)]
    else:
        pos_rel = add(add([z*v for v in up1],[x*v for v in fwd1]),[r*v for v in rot1])
    return add(pos_rel,ref)


def legPos(ileg,x=0,z=0,ref=base):
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
        self.pos = bodyPos(x=x,z=z,r=r,ref=self.pos)
        return self.pos
    def legMove(self,ileg,x=0,z=0):
        self.pos = legPos(ileg,x=x,z=z,ref=self.pos)
        return self.pos
    def set(self,pos):
        self.pos=pos
        return self.pos
    def get(self,):
        return self.pos

up = bodyPos(z=40)
down = bodyPos(z=-40)
fwd = bodyPos(x=40)
back = bodyPos(x=-40)

title('Zero')
print set(zero)

title('Base')
print set(base)

title('Up')
print set(up)

title('Fwd')
print set(fwd)

title('LeftRight')
print clear()
print mv(bodyPos(r=-30))
print mv(bodyPos(r=30))
print mv(base)
print go()

title('Warm up')
#little dance exercising different moves
print clear()
print set(base)

print mv(fwd)
print mv(back)
print mv(base)

print mv(up)
print mv(down)
print mv(base)

print mv(bodyPos(r=20))
print mv(bodyPos(r=-20))
print mv(base)

r=Robot(base)
print mv(r.bodyMove(x=-40)) #back
print mv(r.legMove(0,z=40))
print mv(r.legMove(0,z=-40))
print mv(r.legMove(1,z=40))
print mv(r.legMove(1,z=-40))
print mv(r.bodyMove(x=+80)) #fwd
print mv(r.legMove(2,z=40))
print mv(r.legMove(2,z=-40))
print mv(r.legMove(3,z=40))
print mv(r.legMove(3,z=-40))
print mv(r.set(base)) #base
print go()


title('WalkFlow1')
#continuous body move, with three legs moving on groud and one leg transitionig from the back
h=20
l=8
w0= [0, 3*l,0,-1*l,0, 1*l,0,-3*l]
w01=[0, 2*l,0,-2*l,0, 0*l,h, 0*l]
w1= [0, 1*l,0,-3*l,0,-1*l,0, 3*l]
w12=[0, 0*l,h, 0*l,0,-2*l,0, 2*l]
w2= [0,-1*l,0, 3*l,0,-3*l,0, 1*l]
w23=[0,-2*l,0, 2*l,h, 0*l,0, 0*l]
w3= [0,-3*l,0, 1*l,0, 3*l,0,-1*l]
w30=[h, 0*l,0, 0*l,0, 2*l,0,-2*l]

spd=6
print clear()
print mv(bodyPos(w0),spd)
print mv(bodyPos(w01),spd)
print mv(bodyPos(w1),spd)
print mv(bodyPos(w12),spd)
print mv(bodyPos(w2),spd)
print mv(bodyPos(w23),spd)
print mv(bodyPos(w3),spd)
print mv(bodyPos(w30),spd)
print loop()
print go()

title('WalkFlow2')
#continuous body move, with two legs moving on ground and two transitionig from the back

h=20
l=8
w0=  [0, 3*l,0,-3*l,0,-3*l,0, 3*l]
w01= [0, 0*l,h, 0*l,h, 0*l,0, 0*l]
w1=  [0,-3*l,0, 3*l,0, 3*l,0,-3*l]
w10= [h, 0*l,0, 0*l,0, 0*l,h, 0*l]


spd=6
print clear()
print mv(bodyPos(w0),spd)
print mv(bodyPos(w01),spd)
print mv(bodyPos(w1),spd)
print mv(bodyPos(w10),spd)
print loop()
print go()


title('TurnFlow2')
#continuous turning with two legs on ground and two transitioning
h=20
l=8
w0=  [0, 3*l,0, 3*l,0,-3*l,0, -3*l]
w01= [0, 0*l,h, 0*l,h, 0*l,0, 0*l]
w1=  [0,-3*l,0,-3*l,0, 3*l, 0,3*l]
w10= [h, 0*l,0, 0*l,0, 0*l,h, 0*l]


spd=6
print clear()
print mv(bodyPos(w0),spd)
print mv(bodyPos(w01),spd)
print mv(bodyPos(w1),spd)
print mv(bodyPos(w10),spd)
print loop()
print go()


title('Jump4')
#it do not have enough power to lift off on the jump :(
h=30
l=8
w0=  [ 0, 0*l, 0, 0*l, 0, 0*l, 0, 0*l]
w01= [ h, 0*l, h, 0*l, h, 0*l, h, 0*l]
w1=  [-h, 0*l,-h, 0*l,-h, 0*l,-h, 0*l]
w10= [ 0, 0*l, 0, 0*l, 0, 0*l, 0, 0*l]


spd=2
print clear()
print mv(bodyPos(w0),spd)
print mv(bodyPos(w01),spd)
print mv(bodyPos(w1),1000)
print mv(bodyPos(w10),spd)
print loop()
print go()
