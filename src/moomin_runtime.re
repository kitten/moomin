open Moomin_types;
open Moomin_react_types;
open Moomin_react;

let render = (root: elementT): reprocessingLoopT(recNodeT) => {
  let rec unmount = (recNode: recNodeT) => {
    switch (recNode) {
    | R_CHILD(path, element, recNode) => {
      unmount(recNode);
      element.unmount(~path);
    }
    | R_CHILDREN(recNodes) =>
      StateMap.forEach(recNodes, (_key, recNode) => unmount(recNode))
    | R_NULL => ()
    }
  };

  let rec traverse = (
    ~path="root",
    ~index=0,
    ~prevRec=R_NULL,
    glEnv: Reprocessing.glEnvT,
    children: childrenT
  ) => {
    switch (children) {
    | C_SINGLE(element) => {
      Reprocessing.Draw.pushStyle(glEnv);
      Reprocessing.Draw.pushMatrix(glEnv);

      let path = path ++ ":" ++ getElementKey(element, index);

      let childRec = switch (prevRec) {
      | R_CHILD(x, _, childRec) when x === path => childRec
      | _ => {
        unmount(prevRec);
        element.initialState(~path, ~glEnv);
        R_NULL
      }
      };

      let child = element.render(~path, ~glEnv);
      let recNode = traverse(~path, ~prevRec=childRec, glEnv, child);

      Reprocessing.Draw.popStyle(glEnv);
      Reprocessing.Draw.popMatrix(glEnv);

      R_CHILD(path, element, recNode)
    }
    | C_MANY(elements) => {
      let recMap = StateMap.make();
      let prevRecMap = switch (prevRec) {
      | R_CHILDREN(recMap) => recMap
      | _ => {
        unmount(prevRec);
        StateMap.make()
      }
      };

      Array.iteri((i, element) => {
        let path = path ++ ":" ++ getElementKey(element, i);
        let prevRec = getPop(prevRecMap, path, R_NULL);
        let recNode = traverse(~path, ~prevRec, ~index=i, glEnv, C_SINGLE(element));
        StateMap.set(recMap, path, recNode);
      }, elements);

      unmount(prevRec);
      R_CHILDREN(recMap)
    }
    | C_NULL => R_NULL
    }
  };

  {
    setup: glEnv => {
      Reprocessing.Env.size(~width=200, ~height=200, glEnv);
      Reprocessing.Draw.fill(Moomin_colors.transparent, glEnv);
      Reprocessing.Draw.stroke(Moomin_colors.transparent, glEnv);
      Reprocessing.Draw.background(Moomin_colors.white, glEnv);
      R_NULL
    },
    draw: (state, glEnv) => {
      Reprocessing.Draw.background(Moomin_colors.white, glEnv);
      traverse(~prevRec=state, glEnv, C_SINGLE(root))
    }
  }
};
