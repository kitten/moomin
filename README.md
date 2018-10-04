# Moomin

An **experimental** framework around [Reprocessing](https://github.com/Schmavery/reprocessing) for Reason,
allowing games to be written with components and JSX.

> **NOTE:** This is an experimental stage project. It's basic functionality is to provide components
> that can render SVG-like elements with Reprocessing.
> It's currently just a lean alternative API around Reprocessing that doesn't do much more than to
> organise state in components and to allow JSX instead of direct function calls.

<br>

<center><img alt="Moomin" src="/docs/moomin.jpg?raw=true" width="300px" /></center>

## Why?

[Reprocessing](https://github.com/Schmavery/reprocessing) is an excellent library for writing games
with as little code as is possible. Out of sheer interest, I wanted to see whether it was feasible
to structure games like you would structure a React app.

The more interesting implication of this is that someone familiar with React will be able to write
cleaner games, if they have access to a component/element tree and an SVG-like JSX syntax.

On the other hand it does _complicate_ Reprocessing's minimal and elegant API, and you might find
it to add mental overhead.

## How?

This library follows [ReasonReact's](https://reasonml.github.io/reason-react/docs/en/jsx) structure
and utilises Reason's built-in JSX transpilation to create elements of its own.
Instead of creating elements and components for React to use, it instead has its own element structure.

Once elements in Moomin are created however, it follows the React intuition quite closely. Every
element is rendered at 60 FPS (Not when state changes!) and preserves its state across renders.

When an element unmounts (or its key changes, like in React) its state is thrown away. The usual
lifecycle rules of React apply and ReasonReact's API is adopted vaguely, but not closely.

## Table of Contents

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
<!-- END doctoc generated TOC please keep comment here to allow auto update -->

- [Installation](#installation)
- [Usage](#usage)
  - [Basic Example](#basic-example)
  - [Stateful Example](#stateful-example)
- [Basics](#basics)
  - [Components](#components)
  - [Lifecycles](#lifecycles)
  - ["DOM" Elements](#dom-elements)
- [Shortcomings / Plans](#shortcomings--plans)
- [API](#api)
  - [Basic JSX](#basic-jsx)
  - [Base Elements Props](#base-elements-props)
  - [Elements](#elements)
    - [`<g>`](#g)
    - [`<rect>`](#rect)
    - [`<rect>`](#rect-1)
    - [`<line>`](#line)
    - [`<triangle>`](#triangle)
    - [`<ellipse>`](#ellipse)
    - [`<text>`](#text)
    - [`<image>`](#image)
  - [Component Creation](#component-creation)
  - [Component & Element Types](#component--element-types)
    - [`selfT`](#selft)
    - [`componentT`](#componentt)
  - [Component API](#component-api)
    - [`initialState`](#initialstate)
    - [`willUpdate`](#willupdate)
    - [`render`](#render)
    - [`didMount`](#didmount)
    - [`didUpdate`](#didupdate)
    - [`willUnmount`](#willunmount)
    - [`reducer`](#reducer)
  - [Standard Components](#standard-components)
    - [`<Std.Setup>`](#stdsetup)
  - [Reprocessing APIs](#reprocessing-apis)
- [LICENSE](#license)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Installation

This package is distributed via npm which is bundled with node and
should be installed as one of your project's `dependencies`:

```sh
# npm
npm install --save moomin
# yarn
yarn add moomin
```

After installing the package, don't forget to add it to your `bsconfig.json` and
make sure to enable Reason's `react-jsx: 2` mode:

```diff
{
  "name": "<your name>",
  "version": "0.1.0",
  "sources": ["src"],
  "bsc-flags": ["-bs-super-errors"],
  "bs-dependencies": [
+    "moomin"
  ],
+  "refmt": 3,
+  "reason": {
+    "react-jsx": 2
+  }
}
```

> This package does not depend on Reprocessing directly as it vendors it
> for now.
>
> - Please make sure not to install and add it to your `bsconfig.json`.
> - Please make sure not to add ReasonReact as well, as its module names are taken up by Moomin.

## Usage

### Basic Example

Here's a simple example with a single component rendering two rectangles on screen:

```reason
open Moomin;

module Example = {
  let component = ReasonReact.statelessComponent("Example");

  let make = (~x, ~y, _children) => {
    ...component,
    render: self => {
      <>
        <rect x={x} y={y} width={50.} height={50.} fill={Colors.blue} />
        <rect x={x} y={y +. 100.} width={50.} height={50.} fill={Colors.blue} />
      </>
    }
  };
};

run(
  <Std.Setup width={200} height={200} background={Colors.white}>
    <Example x={25.} y={25.} />
    <Example x={125.} y={25.} />
  </Std.Setup>
);
```

As you can see here, the API closely follows `ReasonReact`'s API, with the familiar
component creation and JSX.

The `render` lifecycle in this example returns two `<rect>` elements inside a fragment.
Similar to SVG we'd also be able to wrap them inside a `<g>`, which allows for some
transformations.

Lastly the `run` function accepts some JSX elements and starts the Reprocessing
render loop with all elements' lifecycles.

### Stateful Example

Stateful components ("reducer components") follow the same practices.

```reason
module Square = {
  type state = {
    rotate: float
  };

  let component = ReasonReact.reducerComponent("Square");

  let make = (_children) => {
    ...component,
    initialState: _glEnv => { rotate: 0. },
    reducer: (_action: unit, state) => state,
    willUpdate: self => { rotate: self.state.rotate +. 0.1 },
    render: self => {
      <rect
        x={5.}
        y={5.}
        width={20.}
        height={20.}
        rotate={self.state.rotate}
        fill={Colors.black}
      />
    }
  };
};
```

You might notice that some lifecycles are different from ReasonReact's ones.
Due to the fact that render cycles in Moomin occur at up to 60 times per second,
this also means that some lifecycles from ReasonReact don't quite fit this usecase.

More on this in the next section, "Basics".

## Basics

### Components

Similarly to `ReasonReact` there's helper functions to create a component
"template". In Moomin there's (currently) only two:

- `ReasonReact.statelessComponent`
- `ReasonReact.reducerComponent`.

These two functions accept a _component name_ as their only argument. Unlike in
`ReasonReact` these names must be unique and are not for debugging only, as they're
also used for the reconciliation process.

The last two examples already illustrate how to create stateless and stateful components.

### Lifecycles

Moomin's components have different lifecycle methods from `ReasonReact`'s ones. They're
similar but slightly different. To be specific there are:

- `initialState`
- `willUpdate`
- `render`
- `didMount`
- `didUpdate`
- `willUnmount`

Some of these might already imply their general use (more on that in the API section).
All of these methods are called synchronously during rendering, so there's no special
consideration being made for concurrency, hence `willUnmount` is just a simple lifecycle
method for instance.

### "DOM" Elements

Moomin also exposes a `ReactDOMRe` module. It obviously doesn't render to the DOM, but it
has some elements that might remind you of SVG elements, namely:

- `<rect>`
- `<image>`
- `<line>`
- `<triangle>`
- `<ellipse>`
- `<circle>`
- `<text>`
- `<g>`

Not all SVG elements have been implemented and some (Looking at you `<triangle>`!) are
not SVG elements at all.

## Shortcomings / Plans

- Complex SVG elements have not been implemented. `<path>` for instance
- There's no element for tilemaps yet
- There's no new color utilities for hex colours and more
- Animations are a consideration, but efforts aren't complete yet (See `moomin_animated.re`)
- Events are a consideration, but haven't been implemented yet
- There's no components in `Std` for some standard timing or input events

## API

### Basic JSX

The basic use of components and JSX follows ReasonReact. Take a look at their
[docs on JSX](https://reasonml.github.io/reason-react/docs/en/jsx) for more information.

The entrypoint for your game will always be the `run()` function, which accepts a JSX
element.

### Base Elements Props

All "DOM" elements accept a common set of props. All their styles and transformations will
cascade down the element tree, meaning that when `<g>` has some props applied, those props
will also affect its children.

To summarise these common props affect the current element and its children, but never
sibling elements or parents.

| Prop            | Type         | Description                                   |
| --------------- | ------------ | --------------------------------------------- |
| `fill`          | `colorT`     | Changes the current fill colour               |
| `stroke`        | `colorT`     | Changes the current stroke colour             |
| `strokeWidth`   | `int`        | Changes the current stroke width              |
| `strokeLinecap` | `strokeCapT` | Changes the current stroke cap style          |
| `x`             | `float`      | Moves the drawing starting point horizontally |
| `y`             | `float`      | Moves the drawing starting point vertically   |
| `rotate`        | `float`      | Rotates the canvas                            |
| `scaleX`        | `float`      | Scales the canvas horizontally                |
| `scaleY`        | `float`      | Scales the canvas vertically                  |
| `shearX`        | `float`      | Shears the canvas horizontally                |
| `shearY`        | `float`      | Shears the canvas vertically                  |

### Elements

These are all "DOM" elements that can be drawn. Remember that all of the above
base props apply to all of these elements.

All of these element's props are technically inside a single shared type, due to
ReasonReact's transpilation limitations. They're also all optional, but mostly
not passing a required prop will default to an appropriate value.

#### `<g>`

The `g` ("group") element accepts all base props but doesn't draw or render anything
on its own. It also accepts any number of children, while all other elements don't
accept any children.

If you don't need to pass any prop to `<g>` you can also just use a fragment (`<>`).

#### `<rect>`

Draws a rectangle.

| Prop     | Type    | Description            |
| -------- | ------- | ---------------------- |
| `width`  | `float` | The rectangle's width  |
| `height` | `float` | The rectangle's height |

#### `<rect>`

Draws a rectangle.

| Prop     | Type    | Description            |
| -------- | ------- | ---------------------- |
| `width`  | `float` | The rectangle's width  |
| `height` | `float` | The rectangle's height |

#### `<line>`

Draws a line. The points still stay relevant to the current coordinate system's translation.
So keep in mind that `x` and `y` will still apply to this element and will move it.

| Prop | Type    | Description                        |
| ---- | ------- | ---------------------------------- |
| `x1` | `float` | Starting point's `x` coordinate    |
| `y1` | `float` | Starting point's `y` coordinate    |
| `x2` | `float` | Destination point's `x` coordinate |
| `y2` | `float` | Destination point's `y` coordinate |

#### `<triangle>`

Draws a triangle. The points still stay relevant to the current coordinate system's translation.
So keep in mind that `x` and `y` will still apply to this element and will move it.

| Prop | Type    | Description                 |
| ---- | ------- | --------------------------- |
| `x1` | `float` | 1st corner's `x` coordinate |
| `y1` | `float` | 1st corner's `y` coordinate |
| `x2` | `float` | 2nd corner's `x` coordinate |
| `y2` | `float` | 2nd corner's `y` coordinate |
| `x3` | `float` | 3rd corner's `x` coordinate |
| `y3` | `float` | 3rd corner's `y` coordinate |

#### `<ellipse>`

Draws an ellipse. The points still stay relevant to the current coordinate system's translation.
So keep in mind that `x` and `y` will still apply to this element and will move it.

| Prop | Type    | Description                         |
| ---- | ------- | ----------------------------------- |
| `cx` | `float` | The ellipse's center `x` coordinate |
| `cy` | `float` | The ellipse's center `y` coordinate |
| `rx` | `float` | The ellipse's horizontal radius     |
| `ry` | `float` | The ellipse's vertical radius       |

#### `<text>`

Draws text. Note that it's not its children that's of type `string`,
but instead it accepts a prop.

| Prop   | Type     | Description                   |
| ------ | -------- | ----------------------------- |
| `font` | `fontT`  | Font used to draw the text    |
| `body` | `string` | The string that will be drawn |

#### `<image>`

Draws an image. Like in Reprocessing itself, if `width` and
`height` are not passed, then the image's resolution is used.

| Prop     | Type     | Description                            |
| -------- | -------- | -------------------------------------- |
| `image`  | `imageT` | The image that should be drawn         |
| `width`  | `float`  | The width at which the image is drawn  |
| `height` | `float`  | The height at which the image is drawn |

### Component Creation

When you create a component there's two helpers you can use:

- statelessComponent
- reducerComponent

Both accept a `string` as an argument, which should be your _unique_ component
name. An exception will be raised if the name is already in use.

The return value of these functions are component templates, which you can then
spread into your `make`'s return value.

### Component & Element Types

#### `selfT`

Multiple component lifecycle methods receive `self` as an argument. The type
of this argument is `selfT`. This is pretty similar (but more simplistic) to
`ReasonReact`'s `self` argument.

```reason
type selfT('state, 'action) = {
  send: 'action => unit,
  state: 'state,
  glEnv: Reprocessing.glEnvT
};
```

`send` is used to dispatch an action to the component's `reducer`, which will
be queued up and run before the next rerender.

`state` is the current state of the mounted component.

`glEnv` is just [Reprocessing's GL Env](https://github.com/Schmavery/reprocessing/blob/master/src/Reprocessing_Env.rei).

#### `componentT`

This is the type of every component.

```reason
type componentSpecT('state, 'action, 'initState) = {
  internal: internalT('state, 'action),
  initialState: Reprocessing.glEnvT => 'initState,
  willUpdate: selfT('state, 'action) => 'state,
  render: selfT('state, 'action) => elementT,
  didMount: selfT('state, 'action) => unit,
  didUpdate: selfT('state, 'action) => unit,
  willUnmount: selfT('state, 'action) => unit,
  reducer: ('action, 'state) => 'state
}
```

### Component API

#### `initialState`

> `Reprocessing.glEnvT => 'initState`

This method receives the [Reprocessing GL Env](https://github.com/Schmavery/reprocessing/blob/master/src/Reprocessing_Env.rei) and should return the initial state of the component.

It's only called before the component is mounted as an element.

#### `willUpdate`

> `selfT => 'state`

This method is called before a (mounted) component is rerendered / redrawn.
It can return new state that will be used during rendering.

#### `render`

> `selfT => elementT`

The well known `render` method returns new elements.
You can use `ReasonReact.null` if you don't wish to return and render
any children. When you want to render multiple elements without having a wrapper
element drawn, use fragments.

More information can be found in [ReasonReact's JSX docs](https://reasonml.github.io/reason-react/docs/en/jsx)
which all apply to Moomin as well.

#### `didMount`

> `selfT => unit`

This method is called after `render` when the component has been mounted for the first time.

#### `didUpdate`

> `selfT => unit`

This method is called after `render` when the (mounted) component has just rerendered.

#### `willUnmount`

> `selfT => unit`

This method is called when a component is unmounting.

#### `reducer`

> `('action, 'state) => 'state`

Unlike in `ReasonReact` the reducer methods in Moomin just return a new version of the state given
an action. This is because there's no need to avoid rerenders since they're happening at 60 FPS anyway.

### Standard Components

Moomin exposes an `Std` module with some "standard" utility components. Currently there's only one.

#### `<Std.Setup>`

This component should be used under all circumstances as it sets up the window / canvas size and
redraws the background colour.

It should wrap all your other elements. See the example for more information

| Prop         | Type              | Description                                         |
| ------------ | ----------------- | --------------------------------------------------- |
| `width`      | `int`             | The desired window / canvas width                   |
| `height`     | `int`             | The desired window / canvas height                  |
| `background` | `colorT`          | The canvas' background colour                       |
| `children`   | `array(elementT)` | Child elements (note that it accepts multiple ones) |

### Reprocessing APIs

Reprocessing's modules are exposed in the same way as they would be if you'd `open Reprocessing`.

You will find:

- `Utils`
- `Constants`
- `Draw`
- `Env`
- `Events`

Apart from those Moomin comes with `Colors` as well, which adds some of its base colours and might
have more utilities in the future.

## LICENSE

MIT
