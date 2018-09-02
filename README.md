tickdb
======

column based in memory database for financial time-series data, embedded with a script language

How to compile
======
1. make
2. run the binary feed a script file like: ./p t/2.t

Script function examples
=======
1. Create in memory data base named 'dbname':
      create('dbname', col_num, 'col_name1', 'col_name2', ...) #  should have col_num's col_name
2. load from csv file to DB:
      appendcsv('dbname', 'xxx.csv',0,1,3,4); # argument after csv file be the used column in the csv file, must be the same length of the DB table.
3. Persistent DB to filesystem.
      write('dbname');
4. Load a persistent DB to memory:
      load('dbname')
5. apply code to specific instrument in db:
      run('symbol')

6. code in {} block are applied to each row in time-series tables, the name before '{' is the program name, the output value will be displayed by db.
  progam_name{
  cl:sum(close, 10);
  }
  
 
operations in code
=======
simple operator and system function which apply to each row in time-series table
 ':' assignment
 ':=' assignment with output
 '+' add
 '-' minus
 '*' multiply
 '/' divide
 sum

look into t/ folder for more examples.
