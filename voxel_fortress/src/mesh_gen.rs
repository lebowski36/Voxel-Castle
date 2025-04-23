use bevy::prelude::*;
use bevy::render::mesh::{Mesh, Indices, PrimitiveTopology};
use bevy::render::render_asset::RenderAssetUsages;

pub fn generate_plane_mesh(size: f32) -> Mesh {
    let half = size / 2.0;
    let vertices = vec![
        [-half, 0.0, -half],
        [half, 0.0, -half],
        [half, 0.0, half],
        [-half, 0.0, half],
    ];
    let indices = Indices::U32(vec![0, 1, 2, 2, 3, 0]);
    let normals = vec![[0.0, 1.0, 0.0]; 4];
    let mut mesh = Mesh::new(PrimitiveTopology::TriangleList, RenderAssetUsages::default());
    mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, vertices);
    mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, normals);
    mesh.insert_indices(indices);
    mesh
}

pub fn generate_cube_mesh(size: f32) -> Mesh {
    let half = size / 2.0;
    let vertices = vec![
        // Front face
        [-half, -half, half],
        [half, -half, half],
        [half, half, half],
        [-half, half, half],
        // Back face
        [-half, -half, -half],
        [half, -half, -half],
        [half, half, -half],
        [-half, half, -half],
    ];
    let indices = Indices::U32(vec![
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        0, 3, 7, 7, 4, 0,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Top face
        3, 2, 6, 6, 7, 3,
        // Bottom face
        0, 4, 5, 5, 1, 0,
    ]);
    let normals = vec![
        [0.0, 0.0, 1.0], // Front
        [0.0, 0.0, -1.0], // Back
        [-1.0, 0.0, 0.0], // Left
        [1.0, 0.0, 0.0], // Right
        [0.0, 1.0, 0.0], // Top
        [0.0, -1.0, 0.0], // Bottom
    ];
    let mut mesh = Mesh::new(PrimitiveTopology::TriangleList, RenderAssetUsages::default());
    mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, vertices);
    mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, normals);
    mesh.insert_indices(indices);
    mesh
}
