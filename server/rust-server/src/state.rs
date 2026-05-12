use crate::models::{Message, User};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::path::Path;
use std::sync::Arc;
use tokio::sync::Mutex;

pub const STATE_FILE: &str = "server_state.json";

fn default_next_message_id() -> u64 {
    1
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ServerData {
    #[serde(default = "default_next_message_id")]
    pub next_message_id: u64,
    #[serde(default)]
    pub users: HashMap<String, User>,
    #[serde(default)]
    pub messages: Vec<Message>,
}

impl Default for ServerData {
    fn default() -> Self {
        Self {
            next_message_id: 1,
            users: HashMap::new(),
            messages: Vec::new(),
        }
    }
}

pub struct AppState {
    // tokio::sync::Mutex используется намеренно — он безопасен для hold через .await
    pub data: Mutex<ServerData>,
    pub start_time: u64,
}

pub type SharedState = Arc<AppState>;

pub fn load_state() -> ServerData {
    let path = Path::new(STATE_FILE);
    if !path.exists() {
        return ServerData::default();
    }
    std::fs::read_to_string(path)
        .ok()
        .and_then(|s| serde_json::from_str(&s).ok())
        .unwrap_or_default()
}

pub fn save_state(data: &ServerData) {
    let content = serde_json::to_string_pretty(data).expect("serialization failed");
    std::fs::write(STATE_FILE, content).expect("failed to save state");
}
