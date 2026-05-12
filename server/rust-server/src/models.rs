use serde::{Deserialize, Serialize};
use serde_json::Value;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct User {
    pub id: u64,
    pub nick: String,
    pub password: String,
    pub created_at: String,
}

impl User {
    pub fn to_public(&self) -> PublicUser {
        PublicUser {
            id: self.id.to_string(),
            nick: self.nick.clone(),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Message {
    pub id: u64,
    pub from_id: u64,
    pub to_id: u64,
    pub text: String,
    pub created_at: String,
}

#[derive(Serialize)]
pub struct PublicUser {
    pub id: String,
    pub nick: String,
}

#[derive(Serialize)]
pub struct PublicMessage {
    pub id: String,
    pub from_id: String,
    pub from_nick: String,
    pub to_id: String,
    pub text: String,
    pub created_at: String,
}

// Принимает id как число или строку — клиент может слать оба варианта
pub fn parse_id(value: Option<&Value>) -> Option<u64> {
    match value? {
        Value::Number(n) => n.as_u64().filter(|&v| v > 0),
        Value::String(s) => s.parse::<u64>().ok().filter(|&v| v > 0),
        _ => None,
    }
}

pub fn parse_id_str(s: &str) -> Option<u64> {
    s.parse::<u64>().ok().filter(|&v| v > 0)
}
