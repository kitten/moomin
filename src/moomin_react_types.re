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
  initialState: (~path: string, ~glEnv: Reprocessing.glEnvT) => unit,
  willReceiveProps: (~path: string, ~glEnv: Reprocessing.glEnvT) => unit,
  willRender: (~path: string, ~glEnv: Reprocessing.glEnvT) => unit,
  render: (~path: string, ~glEnv: Reprocessing.glEnvT) => childrenT,
  didRender: (~path: string, ~glEnv: Reprocessing.glEnvT) => unit,
  willUnmount: (~path: string) => unit
};

type componentSpecT('state, 'action, 'initState) = {
  internal: internalT('state, 'action),
  initialState: Reprocessing.glEnvT => 'initState,
  willReceiveProps: selfT('state, 'action) => 'state,
  willRender: selfT('state, 'action) => unit,
  render: selfT('state, 'action) => elementT,
  didRender: selfT('state, 'action) => unit,
  reducer: ('action, 'state) => 'state
};

type componentT('state, 'action) = componentSpecT('state, 'action, 'state);

type recNodeT =
  | R_NULL
  | R_CHILD(string, elementT, recNodeT)
  | R_CHILDREN(stateMapT(recNodeT));
