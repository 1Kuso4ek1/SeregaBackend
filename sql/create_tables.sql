CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL
);

CREATE TABLE user_keys (
    user_id INT PRIMARY KEY REFERENCES users(id) ON DELETE CASCADE,
    identity_key BYTEA NOT NULL,
    pre_key BYTEA NOT NULL,
    updated_at TIMESTAMPTZ DEFAULT now()
);

CREATE TABLE messages (
    id BIGSERIAL PRIMARY KEY,
    sender_id INT REFERENCES users(id) ON DELETE CASCADE,
    receiver_id INT REFERENCES users(id) ON DELETE CASCADE,
    content BYTEA NOT NULL, -- Encrypted payload
    created_at TIMESTAMPTZ DEFAULT now(),
    delivered BOOLEAN DEFAULT FALSE
);

CREATE TABLE updates (
    id BIGSERIAL PRIMARY KEY,
    user_id INT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    type TEXT NOT NULL,
    payload JSONB NOT NULL
);

-- I'd say that this is some kind of migrations lol
ALTER TABLE messages RENAME COLUMN delivered TO seen;
ALTER TABLE user_keys DROP COLUMN updated_at;
