# Reframe existing atlas FBX. Preview-only; does not modify UE assets.
import bpy,sys,os,math,time
from mathutils import Vector
args=sys.argv[sys.argv.index('--')+1:]
fbx,png=args[:2]
az=float(args[2]) if len(args)>2 else 55.
el=float(args[3]) if len(args)>3 else 27.
zoom=float(args[4]) if len(args)>4 else .68
bpy.ops.wm.read_factory_settings(use_empty=True)
t=time.time(); print("GALATA_FRAME importing FBX",flush=True)
bpy.ops.import_scene.fbx(filepath=fbx)
meshes=[o for o in bpy.data.objects if o.type=='MESH']
if not meshes: raise RuntimeError("FBX has no meshes")
low=Vector((1e12,1e12,1e12));hi=Vector((-1e12,-1e12,-1e12))
for o in meshes:
 for v in o.bound_box:
  p=o.matrix_world@Vector(v)
  for i in range(3):low[i]=min(low[i],p[i]);hi[i]=max(hi[i],p[i])
center=(low+hi)/2;radius=(hi-low).length/2
sc=bpy.context.scene
sc.render.resolution_x=1920;sc.render.resolution_y=1080;sc.render.resolution_percentage=100
camera_data=bpy.data.cameras.new("Galata_Texture_Cam")
cam=bpy.data.objects.new("Galata_Texture_Cam",camera_data);sc.collection.objects.link(cam)
camera_data.lens=40
sensor=camera_data.sensor_width
vfov=2*math.atan((sensor*1080/1920)/(2*camera_data.lens))
hfov=2*math.atan(sensor/(2*camera_data.lens))
dist=radius/math.sin(min(vfov,hfov)/2)*zoom
az=math.radians(az);el=math.radians(el)
cam.location=center+Vector((math.cos(az)*math.cos(el),math.sin(az)*math.cos(el),math.sin(el)))*dist
cam.rotation_euler=(center-cam.location).to_track_quat('-Z','Y').to_euler()
camera_data.clip_start=.05;camera_data.clip_end=dist*10
sc.camera=cam
sc.render.engine='BLENDER_WORKBENCH'
sh=sc.display.shading
sh.light='STUDIO';sh.color_type='TEXTURE'
sh.show_shadows=True;sh.show_cavity=True;sh.cavity_type='WORLD'
sh.background_type='VIEWPORT';sh.background_color=(.52,.61,.72)
try:sh.studio_light='outdoor.sl'
except Exception:pass
try:sc.view_settings.view_transform='Standard'
except Exception:pass
sc.render.film_transparent=False
os.makedirs(os.path.dirname(png),exist_ok=True)
sc.render.filepath=png
print('GALATA_FRAME rendering',radius,zoom,flush=True)
bpy.ops.render.render(write_still=True)
print('GALATA_FRAME DONE',png,os.path.getsize(png),'bytes',round(time.time()-t,1),'sec',flush=True)