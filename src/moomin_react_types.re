module StateMap = Belt.MutableMap.String;
type stateMapT('state) = Belt.MutableMap.String.t('state);

type internalT('state, 'action) = {
  name: string,
  states: stateMapT('state),
  actions: stateMapT(Belt.MutableQueue.t('action))
};

type selfT('state, 'action) = {
  send: 'action => unit,
  state: 'state,
  glEnv: Reprocessing.glEnvT
};

type childrenT =
  | C_NULL
  | C_SINGLE(elementT)
  | C_MANY(array(elementT))
and elementT = {
  name: string,
  key: option(string),
  stateless: bool,
  initialState: (~path: string, ~glEnv: Reprocessing.glEnvT) => unit,
  willUpdate: (~path: string, ~glEnv: Reprocessing.glEnvT) => unit,
  render: (~path: string, ~glEnv: Reprocessing.glEnvT) => childrenT,
  didMount: (~path: string, ~glEnv: Reprocessing.glEnvT) => unit,
  didUpdate: (~path: string, ~glEnv: Reprocessing.glEnvT) => unit,
  willUnmount: (~path: string, ~glEnv: Reprocessing.glEnvT) => unit
};

type componentSpecT('state, 'action, 'initState) = {
  internal: internalT('state, 'action),
  initialState: Reprocessing.glEnvT => 'initState,
  willUpdate: selfT('state, 'action) => 'state,
  render: selfT('state, 'action) => elementT,
  didMount: selfT('state, 'action) => unit,
  didUpdate: selfT('state, 'action) => unit,
  willUnmount: selfT('state, 'action) => unit,
  reducer: ('action, 'state) => 'state
};

type componentT('state, 'action) = componentSpecT('state, 'action, 'state);

type recNodeT =
  | R_NULL
  | R_CHILD(string, elementT, recNodeT)
  | R_CHILDREN(stateMapT(recNodeT));

type reprocessingLoopT('state) = {
  setup: Reprocessing.glEnvT => 'state,
  draw: ('state, Reprocessing.glEnvT) => 'state
};
