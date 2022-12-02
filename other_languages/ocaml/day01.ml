let fold_file filename combiner init =
  let ic = open_in filename
  in let try_read () = try Some (input_line ic) with End_of_file -> None
  in let rec loop acc = match try_read () with Some s -> loop (combiner acc s) | None -> (close_in ic; acc)
  in loop init

let () =
  let injest_into_top3 (top3,v) line =
    let max3 (a,b,c) v =
      match (v>a,v>b,v>c) with
      | (true, _, _) -> (v, a, b)
      | (_, true, _) -> (a, v, b)
      | (_, _, true) -> (a, b, v)
      | _ -> (a, b, c)
    in match int_of_string_opt line with Some x -> (top3, v + x) | None -> (max3 top3 v, 0)
  in let (x, y, z) = fst (fold_file Sys.argv.(1) injest_into_top3 ((0, 0, 0), 0))
  in Printf.printf "%d %d\n" x (x + y + z)