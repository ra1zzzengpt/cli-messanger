use std::sync::Arc;
use std::time::{SystemTime, UNIX_EPOCH};

use axum::{
    routing::{delete, get, patch, post},
    Router,
};
use hyper_util::{
    rt::{TokioExecutor, TokioIo},
    server::conn::auto,
};
use rustls_pemfile::{certs, private_key};
use tokio::sync::Mutex;
use tokio_rustls::{
    rustls::{
        pki_types::{CertificateDer, PrivateKeyDer},
        ServerConfig,
    },
    TlsAcceptor,
};
use tower::util::ServiceExt;

mod error;
mod handlers;
mod models;
mod state;

use handlers::*;
use state::{load_state, AppState};

fn build_tls_acceptor(cert_path: &str, key_path: &str) -> TlsAcceptor {
    let cert_file = std::fs::File::open(cert_path).expect("cannot open cert.pem");
    let key_file = std::fs::File::open(key_path).expect("cannot open key.pem");

    let certs: Vec<CertificateDer<'static>> = certs(&mut std::io::BufReader::new(cert_file))
        .map(|c| c.unwrap())
        .collect();

    let key: PrivateKeyDer<'static> = private_key(&mut std::io::BufReader::new(key_file))
        .unwrap()
        .expect("no private key found in key.pem");

    let config = ServerConfig::builder()
        .with_no_client_auth()
        .with_single_cert(certs, key)
        .unwrap();

    TlsAcceptor::from(Arc::new(config))
}

#[tokio::main]
async fn main() {
    let debug_mode = std::env::var("DEBUG_MODE").unwrap_or_default() == "1";

    if !std::path::Path::new("cert.pem").exists() || !std::path::Path::new("key.pem").exists() {
        eprintln!("ERROR: cert.pem or key.pem not found.");
        eprintln!("Generate a self-signed certificate with:");
        eprintln!(
            "  openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes"
        );
        std::process::exit(1);
    }

    let start_time = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_secs();

    let shared_state = Arc::new(AppState {
        data: Mutex::new(load_state()),
        start_time,
    });

    let mut router = Router::new()
        .route("/ping", get(ping))
        .route("/health", get(health))
        .route("/users/register", post(register_user))
        .route("/users/login", post(login_user))
        .route("/users/:user_id", get(get_user))
        .route("/users/:user_id/nick", patch(update_nick))
        .route("/users/:user_id/password", patch(update_password))
        .route("/messages/send", post(send_message))
        .route("/messages/dump", get(dump_messages))
        .route("/messages", get(fetch_messages));

    if debug_mode {
        router = router
            .route("/debug/state", get(debug_state))
            .route("/debug/reset", delete(debug_reset));
    }

    let app = router.with_state(shared_state);
    let acceptor = build_tls_acceptor("cert.pem", "key.pem");

    let listener = tokio::net::TcpListener::bind("127.0.0.1:5000")
        .await
        .expect("failed to bind port 5000");

    println!("Server running on https://127.0.0.1:5000");
    if debug_mode {
        println!("Debug mode enabled");
    }

    loop {
        let (stream, _) = listener.accept().await.unwrap();
        let acceptor = acceptor.clone();
        let app = app.clone();

        tokio::spawn(async move {
            let tls_stream = match acceptor.accept(stream).await {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("TLS handshake error: {e}");
                    return;
                }
            };

            let io = TokioIo::new(tls_stream);

            // Конвертируем hyper::body::Incoming -> axum::body::Body перед передачей роутеру
            let service = hyper::service::service_fn(move |req: hyper::Request<hyper::body::Incoming>| {
                let req = req.map(axum::body::Body::new);
                app.clone().oneshot(req)
            });

            if let Err(e) = auto::Builder::new(TokioExecutor::new())
                .serve_connection(io, service)
                .await
            {
                eprintln!("connection error: {e}");
            }
        });
    }
}
