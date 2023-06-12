# Jet examples

In this directory you can find some examples of a code written in Jet.

Most of the examples can be compiled and run with:

```sh
jetc [file-name] -o [file-name] && .\[file-name].exe
```

e.g.:

```sh
jetc hello-world.jt -o hello-world && .\hello-world.exe
```

Examples that are put in separate folders with `jetpack.toml` file
require building with the Jetpack tool. Run the following command
in the example's directory:

```sh
jetp build-run .
```

<table>
	<tr>
		<th>Name</th><th>Brief description</th>
	</tr>
	<tr>
		<td><a href="HelloWorld.jt">Hello World</a></td>
		<td>A sample "hello world" program</td>
	</tr>
	<tr>
		<td><a href="Fib.jt">Fibonacci sequence</a></td>
		<td>Prints a Fibonacci sequence up to a user-input number</td>
	</tr>
	<tr>
		<td><a href="GuessANumber.jt">Guess a number</a></td>
		<td>
			The program generates a random number and asks the user
			to guess it
		</td>
	</tr>
</table>