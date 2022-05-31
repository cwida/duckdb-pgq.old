import threading


vertex_init = False
edge_init = False
edge_count = 0
v_lock = threading.Lock()
e_lock = threading.Lock()


class CSR(object):
    def __init__(self, v = [], e = []):
        self.v = v
        self.e = e

def csr_init_v(csr, v_size):
#     csr = pgq.csr[id]
    global vertex_init
#     lock
    v_lock.acquire()
    if(vertex_init):
        v_lock.release()
#         unlock
        return
    csr.v = [0]*v_size
    vertex_init = True
    v_lock.release()
#     unlock
       
# csr.v[0, 0, 2, 4, 5, 7, 8]
def csr_init_e(csr, v_size, e_size):
    global edge_init
#     lock
#     lock = threading.Lock()
    e_lock.acquire()
    if(edge_init):
        e_lock.release()
#         unlock
        return
    csr.e = [0]*e_size
    for i in range(1, v_size + 2):
        csr.v[i] += csr.v[i-1]
    edge_init = True
    print(csr.v)
    e_lock.release()
#     unlock

# csr.v[0, 0, 2, 2, 1, 2, 1]
def create_csr_vertex(csr, v_size, src_list):
    global vertex_init, edge_count
#     csr = pgq.csr[id]
#     lock = threading.Lock()
    if not vertex_init:
        csr_init_v(csr, v_size + 2)
   
    for i in range(len(src_list)):
#         lock
        with v_lock:
            csr.v[src_list[i] + 2] += 1
        edge_count += 1
    return edge_count;
       
# [0, 2, 4, 5, 7, 8, 8]    
def create_csr_edge(csr, e_size, v_size, src_list, dst_list):
#     csr = pgq.csr[id]
    if not edge_init:
        csr_init_e(csr, v_size, e_size)

    for i in range(len(src_list)):
#         lock\
        csr.v[src_list[i]+ 1] += 1
        
        pos = csr.v[src_list[i]+ 1]
        print(pos, i)
        csr.e[pos-1] = dst_list[i]

if __name__=="__main__":
    csr = CSR()
    # v_size_1 = 3
    # e_size_1 = 4
    # v_size_2 = 4
    # e_size_2 = 4
    v_size = 4
    e_size = 4

    # src_list = [0, 0, 1, 1, 2, 3, 3, 4] v_size = 7 e_size = 8
    # src_list = [0, 0, 0, 0, 1, 3, 3, 4] v_size = 5 e_size = 8 dst_list = [1, 2, 3, 4, 2, 2, 4, 1]
    src_list = [0, 0, 1, 2] 
    # [0, 0, 1, 1, 2, 3, 3, 4]
    # dst_list = [1, 3, 6, 4, 5, 0, 2, 5]
    # dst_list = [1, 2, 3, 4, 2, 2, 4, 1]
    dst_list = [1, 2, 2, 3]
    src_list_vertex = [0,1,2,3,4, 5, 6]
    # [1, 3, 6, 4, 5, 0, 2, 5]
#     src_list_1 = [0, 1, 3, 3, 2, 4, 1, 0]
    # src_list_1 = [0, 1, 3]
    # src_list_2 = [3, 2, 4, 1, 0]
#     dst_list = [1, 6, 2, 0, 5, 5, 4, 3]
    # dst_list_1 = [1, 6, 2]
    # dst_list_2 = [0, 5, 5, 4, 3]
#     e_count = create_csr_vertex(csr, v_size, src_list)
#     for i in range(2):
    # t1 = threading.Thread(target=create_csr_vertex, args=(csr, v_size_1, src_list_1,))
    # t2 = threading.Thread(target=create_csr_vertex, args=(csr, v_size_2, src_list_2,))
   
    # t1.start()
    # t2.start()
    # t1.join()
    # t2.join()
    create_csr_vertex(csr, v_size, src_list)
    print(csr.v)
    create_csr_edge(csr, e_size, v_size, src_list, dst_list)
    print(csr.v, csr.e)
#     [0, 2, 4, 5, 7, 8, 8] [1, 3, 6, 4, 5, 0, 2, 5]

