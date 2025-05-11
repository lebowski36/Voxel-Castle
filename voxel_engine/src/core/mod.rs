// Core engine module
// Contains central engine functionality, initialization, and the main loop

mod window;

use anyhow::Result;
use std::time::{Duration, Instant};
use log::{info, trace};
use crate::graphics::Renderer;

pub use window::Window;

/// The main engine struct that coordinates all engine components
pub struct Engine {
    /// Window system
    window: Window,
    /// Rendering system
    renderer: Option<Renderer>,
    /// Whether the engine is running
    running: bool,
    /// Target frames per second
    target_fps: u32,
    /// Frame timing information
    frame_time: Duration,
    /// Last frame timestamp
    last_frame: Instant,
}

impl Engine {
    /// Creates a new engine instance
    pub fn new() -> Result<Self> {
        info!("Initializing engine components...");
        
        // Create window
        let window = Window::new("Voxel Engine")?;

        // Create engine without renderer initially
        let mut engine = Self {
            window,
            renderer: None,
            running: false,
            target_fps: 60,
            frame_time: Duration::from_secs_f32(1.0 / 60.0),
            last_frame: Instant::now(),
        };
        
        // Initialize renderer asynchronously
        pollster::block_on(engine.init_renderer())?;

        Ok(engine)
    }
    
    /// Initializes the renderer asynchronously
    async fn init_renderer(&mut self) -> Result<()> {
        info!("Initializing renderer...");
        let renderer = Renderer::new(self.window.window()).await?;
        self.renderer = Some(renderer);
        Ok(())
    }
    
    /// Runs the main game loop
    pub fn run(mut self) -> Result<()> {
        info!("Starting main loop...");
        
        self.running = true;
        self.last_frame = Instant::now();
        
        while self.running {
            let now = Instant::now();
            let delta_time = now.duration_since(self.last_frame);
            
            if delta_time >= self.frame_time {
                self.last_frame = now;
                
                // Process window events (may set running to false)
                self.window.update(&mut self.running)?;
                
                // Update game state
                self.update(delta_time)?;
                
                // Render frame
                self.render()?;
                
                trace!("Frame time: {:?}", delta_time);
            }
        }
        
        info!("Main loop terminated");
        Ok(())
    }
    
    /// Updates the game state for a frame
    fn update(&mut self, delta_time: Duration) -> Result<()> {
        // We'll add more update logic as we build the engine
        Ok(())
    }
    
    /// Renders a frame
    fn render(&mut self) -> Result<()> {
        if let Some(renderer) = &mut self.renderer {
            renderer.render()?;
        }
        Ok(())
    }
}
