# Atlas Generation Files - Status Guide
*Updated: 2025-06-08*

## 🎯 OFFICIAL CURRENT FILE

**`create_atlas_official.py`** - THE ONLY FILE YOU SHOULD USE
- Dynamic multi-file atlas support
- JSON-driven block data
- Part of Unified Block Resource System Phase 2
- Generates: atlas_main.png, atlas_side.png, atlas_bottom.png + metadata

**Usage:**
```bash
python create_atlas_official.py
```

## 📁 Legacy/Development Files (DO NOT USE)

These files are kept for reference but should NOT be used:

- `create_atlas.py` - Original hardcoded BLOCK_MAPPING system ❌ DEPRECATED
- `create_atlas_with_json.py` - Early JSON integration attempt ❌ DEPRECATED  
- `create_atlas_dynamic.py` - Development version of official ❌ DEPRECATED
- `create_atlas_backup.py` - Legacy backup ❌ DEPRECATED
- `create_atlas_new.py` - Early development ❌ DEPRECATED
- `create_atlas_efficient.py` - Face-based system prototype ❌ DEPRECATED
- `create_atlas_modular.py` - Modular texture experiments ❌ DEPRECATED
- `create_debug_atlas.py` - Debug/testing only ❌ DEPRECATED

## 🗂️ File Cleanup Plan

These legacy files should be moved to a `legacy/` folder to avoid confusion:

```bash
mkdir legacy
mv create_atlas.py legacy/
mv create_atlas_with_json.py legacy/
mv create_atlas_dynamic.py legacy/
mv create_atlas_backup.py legacy/
mv create_atlas_new.py legacy/
mv create_atlas_efficient.py legacy/
mv create_atlas_modular.py legacy/
mv create_debug_atlas.py legacy/
```

## 🎯 Clear Development Path

1. **Current (Phase 2)**: `create_atlas_official.py` - Dynamic multi-file system ✅
2. **Next (Phase 3)**: Code generation integration (C++ headers, build system)
3. **Future (Phase 4)**: Engine integration with dynamic atlas loading

## 🔄 Related Systems

- **Block Data**: `data/blocks/*.json` - Unified block definitions
- **Atlas Metadata**: `assets/textures/atlas_metadata.json` - Generated atlas info
- **Face Patterns**: `atlas_face_system.py` - Face pattern logic
- **Texture Generation**: `texture_generators/` - Modular texture system

---

**REMEMBER**: Only use `create_atlas_official.py` for atlas generation!
