// Window management module
// Handles window creation and event processing using winit

use anyhow::Result;
use winit::{
    event::{Event, WindowEvent},
    event_loop::{ControlFlow, EventLoop},
    window::{Window as WinitWindow, WindowBuilder},
};
use log::{debug, info};

/// Window management system
pub struct Window {
    event_loop: EventLoop<()>,
    window: WinitWindow,
}

impl Window {
    /// Creates a new window with the given title
    pub fn new(title: &str) -> Result<Self> {
        info!("Creating window with title: {}", title);
        
        let event_loop = EventLoop::new();
        let window = WindowBuilder::new()
            .with_title(title)
            .with_inner_size(winit::dpi::LogicalSize::new(1280, 720))
            .build(&event_loop)?;
            
        debug!("Window created successfully");
        
        Ok(Self {
            event_loop,
            window,
        })
    }
    
    /// Updates the window and processes events
    /// Returns whether the application should continue running
    pub fn update(&mut self, running: &mut bool) -> Result<()> {
        // Process events
        self.event_loop.run_return(|event, _, control_flow| {
            *control_flow = ControlFlow::Poll;
            
            match event {
                Event::WindowEvent { 
                    event: WindowEvent::CloseRequested,
                    .. 
                } => {
                    debug!("Window close requested");
                    *control_flow = ControlFlow::Exit;
                    *running = false;
                },
                Event::MainEventsCleared => {
                    // Application update logic would go here
                    *control_flow = ControlFlow::Exit; // Break from this event loop iteration
                },
                _ => {},
            }
        });
        
        Ok(())
    }
    
    /// Returns a reference to the underlying winit window
    pub fn window(&self) -> &WinitWindow {
        &self.window
    }
}
