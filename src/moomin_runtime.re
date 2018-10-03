open Moomin_react_types;
open Moomin_react;

type reprocessingLoopT('state) = {
  setup: Reprocessing.glEnvT => 'state,
  draw: ('state, Reprocessing.glEnvT) => 'state
};

let getPop = (map: stateMapT('a), key: string, default: 'a) =>
  switch (StateMap.get(map, key)) {
  | Some(res) => {
    StateMap.remove(map, key);
    res
  }
  | None => default
  };

let render = (root: elementT): reprocessingLoopT(recNodeT) => {
  let rec unmount = (~glEnv: Reprocessing.glEnvT, recNode: recNodeT) => {
    switch (recNode) {
    | R_CHILD(path, element, recNode) => {
      unmount(~glEnv, recNode);
      element.willUnmount(~path, ~glEnv);
    }
    | R_CHILDREN(recNodes) =>
      StateMap.forEach(recNodes, (_key, recNode) => unmount(~glEnv, recNode))
    | R_NULL => ()
    }
  };

  let rec renderChild = (
    ~prevRec=R_NULL,
    ~path: string,
    glEnv: Reprocessing.glEnvT,
    element: elementT
  ) => {
    Reprocessing.Draw.pushStyle(glEnv);
    Reprocessing.Draw.pushMatrix(glEnv);

    let (isMounting, childRec) = switch (prevRec) {
    | R_CHILD(x, _, childRec) when x === path => {
      element.willUpdate(~path, ~glEnv);
      (false, childRec)
    }
    | _ => {
      unmount(~glEnv, prevRec);
      element.initialState(~path, ~glEnv);
      (true, R_NULL)
    }
    };

    let child = element.render(~path, ~glEnv);
    let recNode = renderTree(~path, ~prevRec=childRec, glEnv, child);

    if (isMounting) {
      element.didMount(~path, ~glEnv);
    } else {
      element.didUpdate(~path, ~glEnv);
    };

    Reprocessing.Draw.popStyle(glEnv);
    Reprocessing.Draw.popMatrix(glEnv);

    R_CHILD(path, element, recNode)
  } and renderTree = (
    ~path="root",
    ~index=0,
    ~prevRec=R_NULL,
    glEnv: Reprocessing.glEnvT,
    children: childrenT
  ) => {
    switch (children) {
    | C_SINGLE(element) => {
      let path = path ++ ":" ++ getElementKey(element, index);
      renderChild(~prevRec, ~path, glEnv, element)
    }
    | C_MANY(elements) => {
      let recMap = StateMap.make();
      let prevRecMap = switch (prevRec) {
      | R_CHILDREN(recMap) => recMap
      | _ => {
        unmount(~glEnv, prevRec);
        StateMap.make()
      }
      };

      Array.iteri((i, element) => {
        let path = path ++ ":" ++ getElementKey(element, i);
        let prevRec = getPop(prevRecMap, path, R_NULL);
        let recNode = renderChild(~prevRec, ~path, glEnv, element);
        StateMap.set(recMap, path, recNode);
      }, elements);

      unmount(~glEnv, prevRec);
      R_CHILDREN(recMap)
    }
    | C_NULL => R_NULL
    }
  };

  {
    setup: glEnv => {
      Reprocessing.Draw.fill(Moomin_colors.transparent, glEnv);
      Reprocessing.Draw.stroke(Moomin_colors.transparent, glEnv);
      renderTree(glEnv, C_SINGLE(root))
    },
    draw: (state, glEnv) => {
      renderTree(~prevRec=state, glEnv, C_SINGLE(root))
    }
  }
};
