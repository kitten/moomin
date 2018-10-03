open Moomin_react_types;
open Moomin_react_helpers;

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
  ComponentNames.checkName(name);

  {
    name,
    states: StateMap.make(),
    actions: StateMap.make()
  }
};

let getElementKey = (element: elementT, index: int) =>
  element.name ++ "(" ++ switch (element.key) {
  | Some(x) => x
  | None => string_of_int(index)
  } ++ ")";

module ReasonReact = {
  let null: elementT = {
    name: "null",
    key: None,
    stateless: true,
    initialState: (~path as _path, ~glEnv as _glEnv) => (),
    willUpdate: (~path as _path, ~glEnv as _glEnv) => (),
    render: (~path as _path, ~glEnv as _glEnv) => C_NULL,
    didMount: (~path as _path, ~glEnv as _glEnv) => (),
    didUpdate: (~path as _path, ~glEnv as _glEnv) => (),
    willUnmount: (~path as _path, ~glEnv as _glEnv) => ()
  };

  let element = (~key=?, component): elementT => {
    let name = component.internal.name;
    let states = component.internal.states;
    let actions = component.internal.actions;

    let makeSelfElement = (~path, ~glEnv) => {
      let state = StateMap.getExn(states, path);
      let actions = StateMap.getExn(actions, path);
      makeSelf(~glEnv, ~state, ~actions)
    };

    let flushActions = (~path, ~glEnv) => {
      let actions = StateMap.getExn(actions, path);
      let state = StateMap.getExn(states, path)
        |> updateState(component.reducer, actions);
      StateMap.set(states, path, state);
      makeSelf(~glEnv, ~state, ~actions);
    };

    {
      name,
      key,
      stateless: false,
      initialState: (~path, ~glEnv) => {
        let state = component.initialState(glEnv);
        StateMap.set(states, path, state);
        StateMap.set(actions, path, Belt.MutableQueue.make());
      },
      willUpdate: (~path, ~glEnv) => {
        let self = makeSelfElement(~path, ~glEnv);
        let state = component.willUpdate(self);
        StateMap.set(states, path, state);
      },
      render: (~path, ~glEnv): childrenT => {
        let self = flushActions(~path, ~glEnv);
        let element = component.render(self);
        C_SINGLE(element)
      },
      didMount: (~path, ~glEnv) => {
        let self = makeSelfElement(~path, ~glEnv);
        component.didMount(self);
      },
      didUpdate: (~path, ~glEnv) => {
        let self = makeSelfElement(~path, ~glEnv);
        component.didUpdate(self);
      },
      willUnmount: (~path, ~glEnv) => {
        let self = makeSelfElement(~path, ~glEnv);
        component.willUnmount(self);
        StateMap.remove(states, path);
        StateMap.remove(actions, path);
      }
    }
  };

  let fragment = "moomin%fragment";

  let statelessComponent = (componentName: string): componentSpecT(unit, unit, unit) => {
    internal: makeInternal(componentName),
    initialState: (_glEnv: Reprocessing.glEnvT): 'initState => (),
    willUpdate: (self: selfT('state, 'action)): 'state => self.state,
    render: (_self: selfT('state, 'action)) => null,
    didMount: (_self: selfT('state, 'action)) => (),
    didUpdate: (_self: selfT('state, 'action)) => (),
    willUnmount: (_self: selfT('state, 'action)) => (),
    reducer: (_action: 'action, state: 'state): 'state => state
  };

  let reducerComponent = (componentName: string): componentSpecT('state, 'action, 'initState) => {
    internal: makeInternal(componentName),
    initialState: (_glEnv: Reprocessing.glEnvT): 'initState => (),
    willUpdate: (self: selfT('state, 'action)): 'state => self.state,
    render: (_self: selfT('state, 'action)) => null,
    didMount: (_self: selfT('state, 'action)) => (),
    didUpdate: (_self: selfT('state, 'action)) => (),
    willUnmount: (_self: selfT('state, 'action)) => (),
    reducer: (_action: 'action, state: 'state): 'state => state
  };
};
