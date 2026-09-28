# Smart Food Delivery System

A command-line food delivery application written in C. It demonstrates modular
programming, validated console input, local persistence, carts and orders,
delivery queues, and shortest-path search on a small delivery graph.

## What It Does

- Admins create and search restaurants, add menu items, view customers and
	orders, update order statuses, manage delivery queues, and view reports.
- Customers register and log in, browse and search restaurants, build a
	single-restaurant cart, place orders, review their orders, track progress, and
	cancel eligible orders.
- The application saves customers, restaurants, orders, and added menu items
	locally in the `data/` directory.

## Requirements

- GCC with C11 support. MinGW-w64 is suitable for Windows.
- A terminal or command prompt. Windows Terminal and the VS Code integrated
	terminal are supported; the interface falls back to plain text when output is
	redirected.

No third-party libraries are required.

## Build and Run

Open PowerShell in the project directory and run:

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic *.c -o SmartFoodDelivery.exe
.\SmartFoodDelivery.exe
```

Compile **all** `.c` files together. Compiling only `main.c` will not link the
other project modules. Run the program from the project directory because its
save paths are relative to the current working directory. If Windows says the
executable is in use, exit the running application before rebuilding.

## First-Time Setup

1. Choose **Admin login**. The demo credentials are username `admin` and
	 password `admin123`.
2. Add restaurants from the admin menu. Restaurant IDs are assigned in order.
3. Built-in menu items are associated with restaurant IDs 1, 2, and 3. Use
	 **Add a menu item** for restaurants that do not have a menu.
4. Return to the main menu and open the customer portal.
5. Create a customer account, log in, select a restaurant, add items, and check
	 out.

On a clean clone with no `data/customers.dat`, 20 synthetic demo accounts are
loaded from `data/sample_customers.csv`. Their usernames are `demo01` through
`demo20`; passwords are `DemoOnly01` through `DemoOnly20`. The first real
account save creates `customers.dat`, which then takes precedence over the
sample file.

For the complete menu reference, workflows, data-file details, architecture,
and troubleshooting, see [PROJECT_DOCUMENTATION.md](PROJECT_DOCUMENTATION.md).

## Important Notes

- Menu choices are numeric. Text fields accept spaces; invalid or oversized
	input is rejected and prompted again.
- The FIFO queue exists only for the current run. Pending priority-queue entries
	are rebuilt from saved orders when the program starts.
- The `data/*.dat` files contain local binary records and are ignored by Git.
	Back them up before moving, replacing, or deleting the project.
- Only synthetic sample data is tracked under `data/`; local runtime databases
	are intentionally not pushed.
- This is a learning/demo application, not a production service. Customer
	passwords are stored as plain text, and the admin credentials are hard-coded.
	Do not use real passwords or sensitive personal information.

## Verification

Build with warnings enabled using the command above. There is no dedicated test
runner yet; the project has been checked with scripted console workflows for
registration, ordering, menu persistence, delivery queues, status changes,
tracking, reporting, and route lookup.