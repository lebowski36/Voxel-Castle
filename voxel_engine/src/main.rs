mod core;
mod graphics;
mod ecs;
mod voxel;
mod utils;

use anyhow::Result;
use log::info;

fn main() -> Result<()> {
    env_logger::init();
    
    info!("Starting Voxel Engine...");
    
    // Initialize engine components
    let engine = core::Engine::new()?;
    
    // Run the main game loop
    engine.run()?;
    
    info!("Shutting down Voxel Engine...");
    
    Ok(())
}
