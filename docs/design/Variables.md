# Variables <small>(Jet » Design)</small>

## Overview

Variables are created with the `let` keyword. The type of the
variable is inferred from the value assigned to it.
The type can be also explicitly specified with the `: [type]` syntax.

```jet
fn main {
  let age: i32 = 1;
  let name = "Jet"; // inferred: str
}
```

Variables are immutable by default:

```jet
fn main {
  let age = 1;
  age = 2; // error
}
```

To create a mutable variable use a `var` keyword instead of `let`.

```jet
fn main {
  var age = 1;
  age = 2;
}
```

## References

To reference an object, use the `&` symbol:

```jet
fn main {
  let age = 1;
  let age_ref = &age;
}
```

References behave like a non-null pointer equivalent in C++
with implicit dereference.

```jet
fn main {
  let age = 1;
  let age_ref = &age;
  println("Age: {}", age_ref); // prints "Age: 1"
}
```

References can be reassigned:

```jet
fn main {
  let x = 1;
  let y = 2;
  let r = &x;
  // rebind to y
  r = &y;
  println("Age: {}", r); // prints "Age: 2"
}
```