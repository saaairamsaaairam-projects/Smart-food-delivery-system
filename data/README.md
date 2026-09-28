# Demo Data

`sample_customers.csv` contains 20 synthetic demo-only customer accounts. The
application loads these accounts only when `customers.dat` does not exist.
Creating or logging in with real accounts creates the local `customers.dat`
database, which takes precedence over the sample file.

Demo account credentials follow this pattern:

- Usernames: `demo01` through `demo20`
- Passwords: `DemoOnly01` through `DemoOnly20`

All names, phone numbers, and addresses in this file are fictional placeholders.
Never put real customer details or passwords in this sample file or in Git.
Runtime `.dat` files are intentionally ignored by Git.