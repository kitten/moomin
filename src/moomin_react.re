open Moomin_react_types;

let makeSelf = (
  ~glEnv: Reprocessing.glEnvT,
  ~state: 'state,
  ~actions: Belt.MutableQueue.t('action)
) => {
  send: (action: 'action) => Belt.MutableQueue.add(actions, action),
  state: state,
  glEnv: glEnv,
};

let makeInternal = (name: string): internalT('state, 'action) => {
  name,
  states: StateMap.make(),
  actions: StateMap.make()
};

let getElementKey = (element: elementT, index: int) =>
  element.name ++ "(" ++ switch (element.key) {
  | Some(x) => x
  | None => string_of_int(index)
  } ++ ")";

let getPop = (map: stateMapT('a), key: string, default: 'a) =>
  switch (StateMap.get(map, key)) {
  | Some(res) => {
    StateMap.remove(map, key);
    res
  }
  | None => default
  };

let rec updateState = (reducer, actions, state) =>
  switch (Belt.MutableQueue.pop(actions)) {
  | Some(action) =>
    updateState(reducer, actions, reducer(action, state))
  | None => state
  };

module ReasonReact = {
  let null: elementT = {
    name: "null",
    key: None,
    initialState: (~path as _path, ~glEnv as _glEnv) => (),
    willReceiveProps: (~path as _path, ~glEnv as _glEnv) => (),
    willRender: (~path as _path, ~glEnv as _glEnv) => (),
    render: (~path as _path, ~glEnv as _glEnv) => C_NULL,
    didRender: (~path as _path, ~glEnv as _glEnv) => (),
    willUnmount: (~path as _path) => ()
  };

  let element = (~key=?, component): elementT => {
    let name = component.internal.name;
    let states = component.internal.states;
    let actions = component.internal.actions;

    {
      name,
      key,
      initialState: (~path, ~glEnv) => {
        let state = component.initialState(glEnv);
        StateMap.set(states, path, state);
        StateMap.set(actions, path, Belt.MutableQueue.make());
      },
      willReceiveProps: (~path, ~glEnv) => {
        let state = StateMap.getExn(states, path);
        let actions = StateMap.getExn(actions, path);
        let self = makeSelf(~glEnv, ~state, ~actions);
        let state = component.willReceiveProps(self);
        StateMap.set(states, path, state);
      },
      willRender: (~path, ~glEnv) => {
        let state = StateMap.getExn(states, path);
        let actions = StateMap.getExn(actions, path);
        let state = updateState(component.reducer, actions, state);
        StateMap.set(states, path, state);
        let self = makeSelf(~glEnv, ~state, ~actions);
        component.willRender(self);
      },
      render: (~path, ~glEnv): childrenT => {
        let state = StateMap.getExn(states, path);
        let actions = StateMap.getExn(actions, path);
        let self = makeSelf(~glEnv, ~state, ~actions);
        let element = component.render(self);
        C_SINGLE(element)
      },
      didRender: (~path, ~glEnv) => {
        let state = StateMap.getExn(states, path);
        let actions = StateMap.getExn(actions, path);
        let self = makeSelf(~glEnv, ~state, ~actions);
        component.didRender(self);
      },
      willUnmount: (~path) => {
        StateMap.remove(states, path);
        StateMap.remove(actions, path);
      }
    }
  };

  let fragment = "moomin%fragment";

  let statelessComponent = (componentName: string): componentSpecT(unit, unit, unit) => {
    internal: makeInternal(componentName),
    initialState: (_glEnv: Reprocessing.glEnvT): 'initState => (),
    willReceiveProps: (self: selfT('state, 'action)): 'state => self.state,
    willRender: (_self: selfT('state, 'action)) => (),
    render: (_self: selfT('state, 'action)) => null,
    didRender: (_self: selfT('state, 'action)) => (),
    reducer: (_action: 'action, state: 'state): 'state => state
  };

  let reducerComponent = (componentName: string): componentSpecT('state, 'action, 'initState) => {
    internal: makeInternal(componentName),
    initialState: (_glEnv: Reprocessing.glEnvT): 'initState => (),
    willReceiveProps: (self: selfT('state, 'action)): 'state => self.state,
    willRender: (_self: selfT('state, 'action)) => (),
    render: (_self: selfT('state, 'action)) => null,
    didRender: (_self: selfT('state, 'action)) => (),
    reducer: (_action: 'action, state: 'state): 'state => state
  };
};
