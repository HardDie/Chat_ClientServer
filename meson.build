project('Chat client server application', 'c')

client_src = ['client.c']
server_src = ['server.c', 'network.c']

List_prj = subproject('List')
List_lib = List_prj.get_variable('l')

ncurses = dependency('ncurses')
pthread = dependency('threads')

executable('server', server_src,
           dependencies : pthread,
           link_with : List_lib)
executable('client', client_src, dependencies : [ pthread, ncurses ])
