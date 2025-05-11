// Renderer module
// Handles wgpu setup, pipelines, and rendering operations

use anyhow::Result;
use log::{debug, info};
use wgpu::{
    Adapter, Backends, Device, Instance, Queue, Surface, SurfaceConfiguration,
    TextureUsages, PresentMode, TextureFormat,
};
use winit::window::Window;

/// The main renderer that handles GPU communication and drawing
pub struct Renderer {
    instance: Instance,
    surface: Surface,
    adapter: Adapter,
    device: Device,
    queue: Queue,
    config: SurfaceConfiguration,
    size: winit::dpi::PhysicalSize<u32>,
}

impl Renderer {
    /// Creates a new renderer for the given window
    pub async fn new(window: &Window) -> Result<Self> {
        info!("Initializing renderer...");
        
        // Create instance
        let instance = Instance::new(wgpu::InstanceDescriptor {
            backends: Backends::all(),
            dx12_shader_compiler: Default::default(),
        });
        
        // Create surface
        let surface = unsafe { instance.create_surface(&window) }?;
        
        // Get adapter
        let adapter = instance
            .request_adapter(&wgpu::RequestAdapterOptions {
                power_preference: wgpu::PowerPreference::default(),
                compatible_surface: Some(&surface),
                force_fallback_adapter: false,
            })
            .await
            .ok_or_else(|| anyhow::anyhow!("Failed to find a suitable GPU adapter"))?;
            
        debug!("Selected adapter: {:?}", adapter.get_info());
        
        // Create device and queue
        let (device, queue) = adapter
            .request_device(
                &wgpu::DeviceDescriptor {
                    label: Some("Primary Device"),
                    features: wgpu::Features::empty(),
                    limits: wgpu::Limits::default(),
                },
                None,
            )
            .await?;
            
        // Configure surface
        let size = window.inner_size();
        let surface_caps = surface.get_capabilities(&adapter);
        let format = surface_caps
            .formats
            .iter()
            .copied()
            .find(|f| f.is_srgb())
            .unwrap_or(surface_caps.formats[0]);
            
        let config = SurfaceConfiguration {
            usage: TextureUsages::RENDER_ATTACHMENT,
            format,
            width: size.width,
            height: size.height,
            present_mode: PresentMode::Fifo,
            alpha_mode: surface_caps.alpha_modes[0],
            view_formats: vec![],
        };
        
        surface.configure(&device, &config);
        
        info!("Renderer initialized successfully");
        
        Ok(Self {
            instance,
            surface,
            adapter,
            device,
            queue,
            config,
            size,
        })
    }
    
    /// Handles window resize events
    pub fn resize(&mut self, new_size: winit::dpi::PhysicalSize<u32>) {
        if new_size.width > 0 && new_size.height > 0 {
            debug!("Resizing renderer to {}x{}", new_size.width, new_size.height);
            self.size = new_size;
            self.config.width = new_size.width;
            self.config.height = new_size.height;
            self.surface.configure(&self.device, &self.config);
        }
    }
    
    /// Renders a frame
    pub fn render(&mut self) -> Result<()> {
        // Get a frame
        let frame = self.surface.get_current_texture()?;
        let view = frame.texture.create_view(&Default::default());
        
        // Create command encoder
        let mut encoder = self.device.create_command_encoder(&wgpu::CommandEncoderDescriptor {
            label: Some("Render Encoder"),
        });
        
        // Begin render pass
        {
            let mut render_pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: Some("Main Render Pass"),
                color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                    view: &view,
                    resolve_target: None,
                    ops: wgpu::Operations {
                        load: wgpu::LoadOp::Clear(wgpu::Color {
                            r: 0.1,
                            g: 0.2,
                            b: 0.3,
                            a: 1.0,
                        }),
                        store: true,
                    },
                })],
                depth_stencil_attachment: None,
            });
            
            // Draw calls would go here
        }
        
        // Submit and present
        self.queue.submit(std::iter::once(encoder.finish()));
        frame.present();
        
        Ok(())
    }
    
    /// Returns a reference to the device
    pub fn device(&self) -> &Device {
        &self.device
    }
    
    /// Returns a reference to the queue
    pub fn queue(&self) -> &Queue {
        &self.queue
    }
}
