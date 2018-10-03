exception DuplicateComponentName(string);

let rec updateState = (reducer, actions, state) =>
  switch (Belt.MutableQueue.pop(actions)) {
  | Some(action) =>
    updateState(reducer, actions, reducer(action, state))
  | None => state
  };

module ComponentNames = {
  let names = Belt.MutableSet.String.make();

  let checkName = (name: string) => {
    if (Belt.MutableSet.String.has(names, name)) {
      raise(DuplicateComponentName(name));
    } else {
      Belt.MutableSet.String.add(names, name);
    }
  };
};
