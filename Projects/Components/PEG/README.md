# PEG <small>(Jet » Comps)</small>

A lightweight and extremely fast PEG parser generator.

## Overview

- no unnecessary dynamic allocations
  - rules are stored in a single buffer
  - text content is stored in a single buffer
  - AST nodes are stored in a single buffer
- grammar is readonly after creation

## Usage

Build a grammar with `GrammarBuilder`

```cpp
auto build_grammar() -> Grammar
{
  auto b = GrammarBuilder();
  
  // "Star" (rule*) - any number of following:
  // true -> capture the rule
  auto root = b.begin_rule(CombinatorRule::Star, true);
  {
    
    // "Sor" (a|b|c|d...) - sequential or
    // true -> capture the rule
    b.begin_rule(CombinatorRule::Sor, true);
    {
      b.add_text("hello");
      b.add_text("world");
    }
    b.end_rule();
  }
  b.end_rule();
  
  return finalize_grammar(root, std::move(b));
}
```

Analyze a document based on the grammar:

```cpp
auto doc = std::string_view("helloworldworldhello");
auto grammar = build_grammar();

auto analysis_result = analyze(doc, grammar);

if (auto failed = analysis_result.err()) {
  // handle failed scenario
  // can still access failed analysis state with
  // failed->state
  return;
}

// success (already checked):
auto& result = analysis_result.get_unchecked();
dump_analysis(grammar, result.state);

// use result.state.ast to access AST
```