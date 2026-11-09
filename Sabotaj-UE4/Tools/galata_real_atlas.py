# Sabotaj / Galata: real-diffuse-texture atlas, Blender 5.2 / UE 4.21-compatible FBX.
# Uses only already-extracted USD and image assets. Existing UE levels not modified.
import bpy, sys, os, math, traceback, gc, time
import numpy as np
from mathutils import Vector
argv=sys.argv[sys.argv.index("--")+1:]
src, outfbx, outpng=argv[:3]
R=float(argv[3]) if len(argv)>3 else 48.0
TARGET=int(argv[4]) if len(argv)>4 else 850000
CELL=256; PAD=3; GRID=16; PER_PAGE=GRID*GRID; INNER=CELL-2*PAD
t0=time.time()
def log(*args): print("[GALATA_REAL]", *args, "elapsed",round(time.time()-t0,1),"s",flush=True)
bpy.ops.wm.read_factory_settings(use_empty=True)
log("USD import begin",src)
bpy.ops.wm.usd_import(filepath=src)
log("USD imported",len(bpy.data.objects),"objects",len(bpy.data.materials),"materials")
def ctr(o):
    bb=[o.matrix_world@Vector(c) for c in o.bound_box]
    return sum(bb,Vector((0,0,0)))/8
def wd(o):
    bb=[o.matrix_world@Vector(c) for c in o.bound_box]
    return max(max(c[i] for c in bb)-min(c[i] for c in bb) for i in range(3))
ms=[o for o in bpy.data.objects if o.type=='MESH' and len(o.data.vertices)>0]
for i in range(6):
    ms.sort(key=wd,reverse=True)
    if len(ms)>2 and wd(ms[0])>3*wd(ms[1]) and wd(ms[0])>300:
        log("Removing giant sky/outlier",ms[0].name,wd(ms[0]))
        bpy.data.objects.remove(ms[0],do_unlink=True);ms=ms[1:]
    else:break
if not ms: raise RuntimeError("No scene meshes")
cs=[ctr(o) for o in ms]
def med(v): v=sorted(v);return v[len(v)//2]
C=Vector([med([v[i] for v in cs]) for i in range(3)])
keep=[o for o,c in zip(ms,cs) if (c-C).length<=R]
if not keep: raise RuntimeError("No meshes selected in radius")
discard=set(ms)-set(keep)
for o in discard:bpy.data.objects.remove(o,do_unlink=True)
log("Selected",len(keep),"meshes, removed",len(discard),"outside radius",R)
bpy.ops.object.select_all(action='DESELECT')
for o in keep:o.select_set(True)
bpy.context.view_layer.objects.active=keep[0]
bpy.ops.object.join()
obj=bpy.context.view_layer.objects.active
me=obj.data
log("Joined",len(me.vertices),"vertices",len(me.polygons),"faces",len(me.materials),"materials",len(me.uv_layers),"uv-layers")
if not me.uv_layers:
    raise RuntimeError("Source USD contains no mesh UVs: cannot make faithful texture atlas")
if len(me.vertices)>TARGET:
    ratio=min(1.0,TARGET/len(me.vertices))
    dec=obj.modifiers.new("UE421_Galata_Decimation","DECIMATE")
    dec.ratio=ratio
    bpy.ops.object.modifier_apply(modifier=dec.name)
    me=obj.data
    log("Decimated to",len(me.vertices),"vertices",len(me.polygons),"faces")
old_mats=list(me.materials)
N=len(old_mats)
if N<1:raise RuntimeError("No source materials")
# Prefer diffuse/albedo image, never mistakenly use a normal-map/ORM as base color.
def select_image(mat):
    if not mat or not mat.use_nodes:return None
    nodes=list(mat.node_tree.nodes)
    tex=[n for n in nodes if n.type=='TEX_IMAGE' and n.image]
    if not tex:return None
    for link in mat.node_tree.links:
        if link.from_node in tex and link.to_socket.name.lower() in ('base color','color','diffuse color'):
            return link.from_node.image
    def quality(n):
        i=n.image
        name=(i.name+" "+os.path.basename(bpy.path.abspath(i.filepath))).lower()
        if any(x in name for x in ("_orm","_normal","_n.","normalmap","normal_map","_mask","_mra","roughness")):return -20
        if any(x in name for x in ("diffuse","albedo","basecolor","base_color","_d.","_bc.","_c.")):return 20
        return 1
    tex.sort(key=quality,reverse=True)
    return tex[0].image if quality(tex[0])>=0 else None
images=[select_image(m) for m in old_mats]
has_image=sum(x is not None for x in images)
log("Materials with real diffuse images",has_image,"/",N)
used=np.empty(len(me.polygons),dtype=np.int32)
me.polygons.foreach_get("material_index",used)
used_count=len(np.unique(used))
log("Used material indices",used_count,"/",N,"UV layer",me.uv_layers.active.name)
os.makedirs(os.path.dirname(outfbx),exist_ok=True)
os.makedirs(os.path.dirname(outpng),exist_ok=True)
def make_tile(i):
    mat=old_mats[i]
    im=images[i]
    if im is not None:
        try:
            if not im.has_data: im.reload()
            size=(int(im.size[0]),int(im.size[1]))
            if min(size)>0:
                if size!=(INNER,INNER): im.scale(INNER,INNER)
                raw=np.empty((INNER,INNER,4),dtype=np.float32)
                im.pixels.foreach_get(raw.reshape(-1))
                if np.isfinite(raw).all():
                    raw[:,:,:]=np.clip(raw,0,1)
                    if np.max(raw[:,:,3])<0.001:raw[:,:,3]=1.0
                    return raw,True
        except Exception as ex:
            log("Texture skipped",i,im.name,str(ex)[:180])
    color=(0.58,0.58,0.58,1.0)
    if mat:
        try:
            if mat.use_nodes:
                for n in mat.node_tree.nodes:
                    if n.type=='BSDF_PRINCIPLED':
                        c=n.inputs['Base Color'].default_value
                        color=tuple(c);break
            else:color=tuple(mat.diffuse_color)
        except Exception:pass
    tile=np.empty((INNER,INNER,4),np.float32)
    tile[:,:,:]=np.array(color,dtype=np.float32)
    tile[:,:,3]=1.0
    return tile,False
n_pages=(N+PER_PAGE-1)//PER_PAGE
atlaspaths=[]
source_success=0
for page in range(n_pages):
    S=GRID*CELL
    canvas=np.ones((S,S,4),dtype=np.float32)
    for i in range(page*PER_PAGE,min(N,(page+1)*PER_PAGE)):
        tile,ok=make_tile(i)
        if ok:source_success+=1
        k=i%PER_PAGE; x=(k%GRID)*CELL; y=(k//GRID)*CELL
        canvas[y+PAD:y+CELL-PAD,x+PAD:x+CELL-PAD,:]=tile
        # Extruded edge gutters eliminate cross-tile seams with mipmaps/filtering.
        canvas[y:y+PAD,x+PAD:x+CELL-PAD,:]=tile[0:1,:,:]
        canvas[y+CELL-PAD:y+CELL,x+PAD:x+CELL-PAD,:]=tile[-1:,:,:]
        canvas[y:y+CELL,x:x+PAD,:]=canvas[y:y+CELL,x+PAD:x+PAD+1,:]
        canvas[y:y+CELL,x+CELL-PAD:x+CELL,:]=canvas[y:y+CELL,x+CELL-PAD-1:x+CELL-PAD,:]
        if (i-page*PER_PAGE+1)%64==0:log("Atlas",page,"filled",i-page*PER_PAGE+1,"slots")
    path=os.path.join(os.path.dirname(outfbx),f"GalataAtlas_{page:02d}.png")
    atl=bpy.data.images.new(f"GalataAtlas_{page:02d}",width=S,height=S,alpha=True)
    atl.filepath_raw=path; atl.file_format='PNG'
    atl.pixels.foreach_set(canvas.ravel())
    atl.save()
    atlaspaths.append(path)
    log("Saved atlas",page,path,os.path.getsize(path),"bytes")
    bpy.data.images.remove(atl)
    del canvas
    gc.collect()
log("Real textures transferred",source_success,"/",N)
# Transform source UV loops into the material's tile of its atlas page (preserves texture pixels).
uv=me.uv_layers.active
loops=np.empty(len(me.polygons),np.int32)
me.polygons.foreach_get("loop_total",loops)
loop_slots=np.repeat(used,loops)
uvflat=np.empty(len(me.loops)*2,dtype=np.float32)
uv.data.foreach_get('uv',uvflat)
uvs=uvflat.reshape((-1,2))
np.remainder(uvs,1.0,out=uvs)
tileids=(loop_slots%PER_PAGE)
tilex=tileids%GRID
tiley=tileids//GRID
scale=(CELL-2*PAD-1)/(GRID*CELL)
uvs[:,0]=(uvs[:,0]* (CELL-2*PAD-1)+tilex*CELL+PAD+0.5)/(GRID*CELL)
uvs[:,1]=(uvs[:,1]* (CELL-2*PAD-1)+tiley*CELL+PAD+0.5)/(GRID*CELL)
uv.data.foreach_set('uv',uvflat)
uv.name='Galata_RealAtlas_UV0'
# All source material slots become 1 material slot per atlas page.
# IMPORTANT: assign polygon material indices AFTER re-adding materials:
# Blender clears/reset face material indices when mesh.materials.clear() is called.
page_slots=used//PER_PAGE
me.materials.clear()
for i,path in enumerate(atlaspaths):
    img=bpy.data.images.load(path,check_existing=False)
    img.pack() if False else None
    m=bpy.data.materials.new(f"M_Galata_RealAtlas_{i:02d}")
    m.use_nodes=True
    nt=m.node_tree;nt.nodes.clear()
    out=nt.nodes.new('ShaderNodeOutputMaterial')
    bsdf=nt.nodes.new('ShaderNodeBsdfPrincipled')
    tex=nt.nodes.new('ShaderNodeTexImage')
    tex.image=img; tex.interpolation='Linear';nt.nodes.active=tex
    nt.links.new(tex.outputs['Color'],bsdf.inputs['Base Color'])
    nt.links.new(bsdf.outputs['BSDF'],out.inputs['Surface'])
    me.materials.append(m)
me.polygons.foreach_set('material_index',page_slots)
log('Atlas face distribution',[(int(i),int(np.count_nonzero(page_slots==i))) for i in range(n_pages)])
me.update()
log("Final mesh",len(me.vertices),"verts",len(me.polygons),"polygons",len(me.materials),"atlas materials",len(me.loops),"loops")
# One static mesh with few materials, physically separate from existing Galata_pal.
bpy.ops.object.select_all(action='DESELECT')
obj.select_set(True);bpy.context.view_layer.objects.active=obj
bpy.ops.export_scene.fbx(filepath=outfbx,object_types={'MESH'},use_selection=True,
                          path_mode='COPY',embed_textures=False,bake_space_transform=False)
log("FBX exported",outfbx,os.path.getsize(outfbx),"bytes")
# Render atlas-textured scene through Workbench texture mode for predictable fast preview.
mn=Vector((1e15,)*3);mx=Vector((-1e15,)*3)
for corner in obj.bound_box:
    p=obj.matrix_world@Vector(corner)
    for axis in range(3):
        mn[axis]=min(mn[axis],p[axis]);mx[axis]=max(mx[axis],p[axis])
center=(mn+mx)/2;radius=(mx-mn).length/2
camdata=bpy.data.cameras.new("GalataPreviewCam")
cam=bpy.data.objects.new("GalataPreviewCam",camdata)
bpy.context.scene.collection.objects.link(cam)
scn=bpy.context.scene
scn.render.resolution_x=1920;scn.render.resolution_y=1080
scn.render.resolution_percentage=100
camdata.lens=40
sensor=camdata.sensor_width
vfov=2*math.atan((sensor*1080/1920)/(2*camdata.lens))
hfov=2*math.atan(sensor/(2*camdata.lens))
dist=radius / math.sin(min(hfov,vfov)/2)*1.15
elev,azim=math.radians(26),math.radians(55)
cam.location=center+Vector((math.cos(elev)*math.cos(azim),math.cos(elev)*math.sin(azim),math.sin(elev)))*dist
cam.rotation_euler=(center-cam.location).to_track_quat('-Z','Y').to_euler()
camdata.clip_start=0.1;camdata.clip_end=dist*8
scn.camera=cam
scn.render.engine='BLENDER_WORKBENCH'
sh=scn.display.shading
sh.light='STUDIO';sh.color_type='TEXTURE'
sh.show_cavity=True;sh.cavity_type='WORLD'
sh.show_shadows=True
try:sh.studio_light='outdoor.sl'
except:pass
try:scn.view_settings.view_transform='Standard'
except:pass
scn.render.film_transparent=False
scn.render.filepath=outpng
log("Rendering source-textured preview",outpng,"center",center[:],"radius",radius)
bpy.ops.render.render(write_still=True)
log("RENDER_OK",outpng,os.path.getsize(outpng),"bytes")
with open(os.path.join(os.path.dirname(outfbx),"Galata_textured_report.txt"),"w",encoding="utf-8") as fh:
    fh.write(f"Source: {src}\nFBX: {outfbx}\nRender: {outpng}\nVertices: {len(me.vertices)}\nFaces: {len(me.polygons)}\nMaterials: {len(me.materials)}\nReal texture slots: {source_success}/{N}\nAtlases: {atlaspaths}\nRuntime sec: {time.time()-t0:.1f}\n")
log("DONE")