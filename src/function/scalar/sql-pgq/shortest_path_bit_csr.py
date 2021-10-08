def empty_visit(a):
    flag = True
    for v,k in a.items():
        if(a[v] == 0):
            continue
        else:
            flag = False
            break
    return flag


if __name__ == "__main__":
    # extra 0 initially as graph is 1 indexed
    csr_v = [0, 0, 2, 4, 7, 10, 11, 12]
    csr_e = [3,4,3,4,1,2,5, 1,2,6, 3, 4]
    w = 2

    # bfs_list = ['b1', 'b2']
    bfs_list = [0b10 , 0b01]
    src_list = [1, 2]

    # neighbours = {1:[3,4], 2:[3,4], 3:[1,2, 5], 4:[1,2,6], 5:[3], 6:[4]}
    seen = {}
    vist = {}
    vist_next = {}

    vertex = [-1, 1,2, 3,4, 5, 6]
    for ele in set(csr_e):
        seen[ele] = 0b00
        vist[ele] = 0b00
        vist_next[ele] = 0b00

    for i in range(len(bfs_list)):
        seen[src_list[i]] = bfs_list[i]
        vist[src_list[i]] = bfs_list[i]
    print("seen " + str(seen))
    print("visit " + str(vist))
    # for i in range(len(bfs_list)):
    #     vist.append((src_list[i], {bfs_list[i]},))
    # print(vist)
    # b = {}

    while not empty_visit(vist):
        for i in range(1, len(vertex)):
            v = vertex[i]
            # v = element[0]
            if not vist.get(v):
                continue
            for n in csr_e[csr_v[v]:csr_v[v+1]]:
                d = vist.get(v) & ~seen[n]
                if d > 0:
                    vist_next[n] = vist_next[n] | d
                    seen[n] = seen[n] | d
        #             can check target here
        vist = vist_next.copy()
        for key,_ in vist_next.items():
            vist_next[key] = 0
        print("visit " + str(vist))
        print("seen" + str(seen))

    
            