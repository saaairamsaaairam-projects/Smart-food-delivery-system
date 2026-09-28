# Smart Food Delivery System: Project Documentation

## 1. Project Overview

Smart Food Delivery System is a local, menu-driven C program for a small food
ordering workflow. An administrator maintains restaurant and menu data and
handles order operations. Customers create accounts, browse menus, place and
track orders, and cancel eligible orders.

The project is designed as a learning/demo application. It uses fixed-size
arrays, separate C modules, text-based interaction, local binary files, FIFO and
priority queues, and a graph shortest-path algorithm. It does not connect to a
network service, payment provider, live map, or restaurant system.

## 2. Goals and Scope

The application demonstrates how to divide a C program into modules with
headers, validate console input, persist records, model relationships using
IDs, and perform basic delivery dispatch and route calculations.

The application currently supports:

- Admin and customer menus.
- Restaurant creation and case-insensitive partial-name search.
- Three built-in restaurant menus and persisted admin-added menu items.
- Customer registration and login.
- One-restaurant-at-a-time carts, checkout, and order history.
- Order status transitions, customer cancellation, and customer-only tracking.
- FIFO and priority delivery queues.
- A fixed sample delivery graph, route display, and shortest-route lookup.
- An admin summary report for order status, priority mix, completed revenue, and
  delivered item count.

## 3. Requirements

### Software

- GCC with C11 support. MinGW-w64 GCC is suitable on Windows.
- A command prompt or terminal that supports standard input/output.
- No external libraries are required.

### Capacity Limits

These limits are defined in the project headers and are intended for a small
local demo:

| Record or collection | Limit |
| --- | ---: |
| Customers | 100 |
| Restaurants | 100 |
| Orders | 100 |
| Items in an order | 20 |
| Items in a cart | 20 |
| Quantity per cart item | 99 |
| Menu items | 500 |
| Entries in either queue | 100 |
| Locations in the delivery graph | 20 |

## 4. Build and Launch

Open PowerShell in the project directory and compile every source file together:

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic *.c -o SmartFoodDelivery.exe
```

Start the application from that same directory:

```powershell
.\SmartFoodDelivery.exe
```

The application opens the main portal menu. Enter the number for an action and
press Enter. The shared input helpers reject invalid numeric values, blank
required fields, and text longer than the target field. End-of-file input exits
the current menu safely.

If GCC reports that it cannot write `SmartFoodDelivery.exe`, close the running
application or build under a different output name. Do not compile only
`main.c`: the project is split across several source files.

## 5. First-Run Setup

### 5.1 Administrator

The demo administrator credentials are:

| Field | Value |
| --- | --- |
| Username | `admin` |
| Password | `admin123` |

These credentials are compiled into `auth.c`; they are not suitable for a
production system.

### 5.2 Add Restaurants and Menus

1. Sign in as the administrator.
2. Choose **Add restaurant** and enter its name, cuisine, location, phone, and
   rating.
3. Restaurant IDs are assigned sequentially, starting at 1.
4. The built-in menus are associated with IDs 1, 2, and 3. Add restaurants in
   the intended order if you want to use those built-in menus.
5. For a restaurant without menu items, choose **Add a menu item**, enter its
   restaurant ID, item name, and price. The application assigns the next item
   ID and saves the menu file.

Creating a restaurant does not automatically create menu items. The customer
order workflow explains when a selected restaurant has no menu configured.

### 5.3 Create a Customer

1. Return to the main menu and choose **Customer portal**.
2. Choose **Create an account** and enter a name, phone, address, username, and
   password.
3. Choose **Log in** and enter the account credentials.

When no `data/customers.dat` exists, the application loads 20 synthetic demo
accounts from `data/sample_customers.csv`. Use usernames `demo01` through
`demo20` and matching passwords `DemoOnly01` through `DemoOnly20`. Once a
customer database is saved, it takes precedence over the sample data.

Names and addresses accept spaces. Customer passwords are currently stored in
plain text; use demo-only credentials.

## 6. Menus and Workflows

### 6.1 Main Menu

| Choice | Action |
| ---: | --- |
| 1 | Open admin login |
| 2 | Open customer portal |
| 0 | Exit |

### 6.2 Customer Portal

| Choice | Action |
| ---: | --- |
| 1 | Create a customer account |
| 2 | Log in |
| 0 | Return to the main menu |

### 6.3 Customer Dashboard

The dashboard displays the customer ID and the current cart item count and
total. The cart is held in memory for the current run.

| Choice | Action |
| ---: | --- |
| 1 | View restaurants |
| 2 | Search by restaurant name; matching ignores case and supports partial names |
| 3 | Select a restaurant, add menu items, and optionally check out |
| 4 | View the signed-in customer's orders |
| 5 | View the cart and optionally clear it |
| 6 | Request cancellation by order ID |
| 7 | Track an order belonging to the signed-in customer |
| 0 | Log out |

A cart can contain items from only one restaurant. Clear it or complete checkout
before ordering from another restaurant. During checkout, select priority 1
(high), 2 (medium), or 3 (low).

### 6.4 Order Status and Cancellation

The supported statuses are:

| Status | Meaning |
| --- | --- |
| `PLACED` | Order has been created |
| `PREPARING` | Restaurant is preparing the order |
| `OUT_FOR_DELIVERY` | Order has been dispatched |
| `DELIVERED` | Delivery is complete |
| `CANCELLED` | Order was cancelled before dispatch |

Allowed changes are:

- `PLACED` to `PREPARING`, `OUT_FOR_DELIVERY`, or `CANCELLED`.
- `PREPARING` to `OUT_FOR_DELIVERY` or `CANCELLED`.
- `OUT_FOR_DELIVERY` to `DELIVERED`.
- `DELIVERED` and `CANCELLED` are final states.

Customers can cancel only their own orders. An order cannot be cancelled after
dispatch. Cancelling, dispatching, or completing an order removes it from active
queues.

### 6.5 Admin Operations

| Choice | Action |
| ---: | --- |
| 1 | Add a restaurant |
| 2 | List restaurants |
| 3 | Search restaurants |
| 4 | List customers |
| 5 | View all orders and their items |
| 6 | Change an order's status, subject to transition rules |
| 7 | Add an eligible order to the FIFO queue |
| 8 | View the FIFO queue |
| 9 | Dispatch the next FIFO order |
| 10 | Add an eligible order to the priority queue |
| 11 | View the priority queue |
| 12 | Dispatch the highest-priority order |
| 13 | View the delivery graph |
| 14 | Find a shortest route between graph locations |
| 15 | Add a menu item to a restaurant |
| 16 | View the operations report |
| 0 | Log out |

Dispatch sets an order to `OUT_FOR_DELIVERY`; it does not mark the order as
delivered. Use **Update order status** to move a dispatched order to
`DELIVERED`.

The operations report counts orders by status and priority. Revenue and item
counts include only delivered orders; this is a simple summary, not accounting
or payment processing.

## 7. Delivery Graph

The graph is initialized with these fixed demo locations:

| ID | Location |
| ---: | --- |
| 0 | Restaurant |
| 1 | MVP Colony |
| 2 | Dwaraka Nagar |
| 3 | RTC Complex |
| 4 | Customer |

Roads are undirected and their distances are configured in `main.c`:

| From | To | Distance |
| --- | --- | ---: |
| Restaurant | MVP Colony | 5 km |
| MVP Colony | Dwaraka Nagar | 3 km |
| Dwaraka Nagar | RTC Complex | 2 km |
| RTC Complex | Customer | 4 km |
| MVP Colony | RTC Complex | 6 km |

The route action uses Dijkstra's algorithm to display a shortest path and total
distance. These locations are currently demonstration data; customer addresses
are not mapped onto this graph, and route calculations are not attached to
individual orders.

## 8. Architecture and Source Files

The program uses module-level arrays for its small fixed-capacity data sets.
Header files declare shared types and functions; source files implement the
behavior.

| File | Responsibility |
| --- | --- |
| `main.c` | Startup, main portal, customer dashboard, admin dashboard, workflow coordination |
| `auth.c`, `auth.h` | Admin login and customer portal navigation |
| `customer.c`, `common.h` | Customer records, registration/login, customer persistence, shared record types and limits |
| `restaurant.c` | Restaurant creation, search, listing, and persistence |
| `menu.c`, `menu.h` | Built-in menu data, added menu items, menu persistence, and item lookup |
| `cart.c`, `cart.h` | Cart operations, totals, restaurant constraint, and checkout entry point |
| `order.c`, `order.h` | Order creation, persistence, status rules, customer history/tracking, operations report |
| `queue.c`, `queue.h` | Session FIFO delivery queue |
| `priority_queue.c`, `priority_queue.h` | Priority ordering and restoration of pending orders |
| `graph.c`, `graph.h` | Delivery graph, shortest distance, and route reconstruction |
| `input.c`, `input.h` | Bounded line, integer, and decimal input validation |
| `console_ui.c`, `console_ui.h` | Shared terminal-aware headers, menu groups, prompts, and progress indicators |

### Startup Sequence

1. Enable terminal colors if stdout is an interactive Windows console.
2. Initialize the delivery queues and graph.
3. Load restaurants and saved menu items.
4. Load orders and rebuild the priority queue for orders not dispatched,
   delivered, or cancelled.
5. Add the fixed demo graph locations and roads.
6. Load customers and show the main portal.

### Console UI Behavior

The UI helper uses ANSI colors only after enabling virtual-terminal processing
on an interactive Windows console. If output is redirected or color support
cannot be enabled, the UI remains plain text. Group labels divide dashboards by
task, while compact headings identify list and detail views.

## 9. Data Storage

All save paths are relative to the working directory. Run the program from the
project directory so the files resolve under `data/`.

| File | Contents |
| --- | --- |
| `data/customers.dat` | Count followed by fixed-size `Customer` records |
| `data/restaurants.dat` | Count followed by fixed-size `Restaurant` records |
| `data/orders.dat` | Count followed by fixed-size `Order` records |
| `data/menus.dat` | Count followed by fixed-size `MenuItem` records; built-in defaults are used when this file is absent |
| `data/sample_customers.csv` | Tracked synthetic demo accounts, used only when no `customers.dat` exists |

The files are native binary data: an integer count followed by raw C structure
bytes. Loaders check counts, record lengths, and selected fields before using
records. This does not make the format portable or versioned. Structure layout,
compiler ABI, architecture, and endianness can affect compatibility.

Customer, restaurant, order, and added-menu records are persisted. Cart state
and the FIFO queue are not persisted. Pending eligible orders are restored into
the priority queue at startup. Back up the `.dat` files before moving,
replacing, or manually editing them; do not open them as text files.

The repository contains only the synthetic sample CSV under `data/`. Runtime
`.dat` databases remain excluded so customer records and passwords are not
accidentally published.

## 10. Limits and Security

- Customer passwords are stored in plain text.
- Admin credentials are hard-coded in the source.
- There is no payment processing, online API, email/SMS notification, or live
  delivery GPS tracking.
- Customer addresses are not linked to graph nodes.
- Restaurant ratings are admin-entered; customer reviews are not implemented.
- Admins can add menu items but cannot edit or deactivate existing records.
- The FIFO queue resets when the program exits.
- Binary persistence is not atomic or schema-versioned; maintain external
  backups.

Do not use real passwords, payment details, or sensitive personal information.
Before production use, add password hashing, configurable admin credentials,
versioned and atomic storage, backups, and access-control review.

## 11. Troubleshooting

| Symptom | Likely cause and resolution |
| --- | --- |
| Undefined references after compiling | Compile every `.c` file with `gcc ... *.c`; do not compile only `main.c`. |
| GCC cannot create the executable | Close the running application or choose a different output filename. |
| No restaurants appear | Sign in as admin and add restaurants first. |
| A restaurant has no menu | Add menu items for that restaurant using admin choice 15. Built-in menus use IDs 1–3. |
| Saved data seems missing | Start the program from the project directory; paths are relative to the current directory. |
| Customer cannot cancel an order | Only the owner can cancel, and orders already out for delivery or completed cannot be cancelled. |
| Colors do not appear | Color is enabled only when the program detects a compatible interactive Windows console; plain text is expected in redirected output. |

## 12. Suggested Next Improvements

Recommended work for a more complete version:

1. Hash passwords and load admin credentials from a protected configuration.
2. Add automated tests for input, order transitions, queue behavior, and file
   compatibility.
3. Add menu and restaurant editing/deactivation while preserving historical
   order details.
4. Add timestamps and map customer addresses to delivery graph nodes.
5. Replace raw structure persistence with a versioned format and atomic backup
   strategy.