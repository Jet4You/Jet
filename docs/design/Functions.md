# Functions <small>(Jet » Design)</small>

## Overview

### Definition

No return, no parameters:

```jet
fn main {
  println("Hello, {} from Jet!", "World");
}
```

No return value, with parameters:

```jet
fn print_sum(a: i32, b: i32) {
  println("{} + {} = {}", a, b, a + b);
}
```

With return value, no parameters:

```jet
fn ask_name -> String {
  ret prompt("What's your name?");
}
```

With return value, with parameters:

```jet
fn add(a: i32, b: i32) -> i32 {
  ret a + b;
}
```

### Calling

Function declaration order is not taken into account:

```jet
fn main {
  let name = ask_name();
  println("Hello, {}!", name);
}

fn ask_name -> String {
  ret prompt("What's your name?");
}
```

### Generic functions

Based on an arbitrary type:

```jet
fn add<T: type>(a: T, b: T) -> T {
  ret a + b;	
}
```

Based on an arbitrary constant:

```jet
fn sum<T: type, BASE: const>(a: T, b: T) -> T {
  ret a + b + BASE;
}
```

Based on an enum:

```jet
enum BookGenre {
  Fantasy,
  SciFi,
  Educational,
}

// `match` operation is prefixed with `$` because
// we're using a compile-time evaluation here.
fn get_book<B: BookGenre, U: type>() -> U {
  ret $match B {
	Fantasy => get_fantasy_book(),
	SciFi => get_scifi_book(),
	Educational => get_educational_book(),
  };
}
```