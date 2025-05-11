use bevy::prelude::*;

// Minimal UI test plugin
pub struct UiTestPlugin;

#[derive(Component)]
pub struct TestUiElement;

impl Plugin for UiTestPlugin {
    fn build(&self, app: &mut App) {
        app.add_systems(Startup, setup_test_ui);
    }
}

// The most basic possible UI implementation following Bevy 0.15.3 documentation
fn setup_test_ui(mut commands: Commands, asset_server: Res<AssetServer>) {
    // Root node
    commands
        .spawn((
            TestUiElement,
            NodeBundle {
                background_color: BackgroundColor(Color::NONE),
                style: Style {
                    width: Val::Percent(100.0),
                    height: Val::Percent(100.0),
                    align_items: AlignItems::Center,
                    justify_content: JustifyContent::Center,
                    ..default()
                },
                ..default()
            },
        ))
        .with_children(|parent| {
            // Text child - simplest possible implementation
            parent.spawn(
                TextBundle::from_sections([
                    TextSection::new(
                        "Test UI",
                        TextStyle {
                            font_size: 30.0,
                            color: Color::WHITE,
                            ..default()
                        }
                    )
                ])
            );
        });
}
