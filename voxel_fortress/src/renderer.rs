pub struct Renderer;

impl Renderer {
    pub fn render(height_map: &[Vec<f64>]) {
        for row in height_map {
            for &value in row {
                let symbol = match value {
                    v if v < -0.5 => '~', // Water
                    v if v < 0.0 => '.',  // Sand
                    v if v < 0.5 => '^',  // Grass
                    _ => 'M',            // Mountain
                };
                print!("{}", symbol);
            }
            println!();
        }
    }
}
