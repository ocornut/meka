program pal2c;
uses lib1;

var f, f2: text;
    s: string;
    n1, n2, n3: integer;

begin;
assign(f, 'o.pal');
reset(f);
assign(f2, 'o.c');
rewrite(f2);
writeln(f2, '{');
while (not eof(f)) do
  begin;
  readln(f, s);
  n1:= str2int (copy(s, 1, pos(' ', s) - 1));
  delete (s, 1, pos(' ', s));
  n2:= str2int (copy(s, 1, pos(' ', s) - 1));
  delete (s, 1, pos(' ', s));
  n3:= str2int (s);
  writeln(f2, ' ' + int2str(n1) + ', ' + int2str(n2) + ', ' + int2str(n3) + ',');
  end;
writeln(f2, '}');
close(f);
close(f2);
end.