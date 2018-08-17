SER_NAME = server
CL_NAME = client

SER_SRC_DIR =	./server_udp/src/
SER_OBJ_DIR =	./server_udp/obj/
SER_INC_DIR =	./server_udp/inc/

CL_SRC_DIR =	./client_udp/src/
CL_OBJ_DIR =	./client_udp/obj/
CL_INC_DIR =	./client_udp/inc/

CF_FLAGS :=	-Wall

SER_HEADER_FLAGS := -I $(SER_INC_DIR)
CL_HEADER_FLAGS := -I $(CL_INC_DIR)

CC :=			gcc

SER_SRC =		server.c\
				get_sockaddr_ip4_or_ip6.c
CL_SRC =		client.c\
				get_sockaddr_ip4_or_ip6.c

SER_OBJ =		$(addprefix $(SER_OBJ_DIR), $(SER_SRC:.c=.o))
CL_OBJ =		$(addprefix $(CL_OBJ_DIR), $(CL_SRC:.c=.o))

all: $(SER_NAME) $(CL_NAME)

$(SER_NAME): $(SER_OBJ)
	@echo $(SER_NAME) ": Source compiling..."
	@$(CC) $(SER_OBJ) -o $(SER_NAME)
	@echo "Executable "$(SER_NAME)" made"

$(CL_NAME): $(CL_OBJ)
	@echo $(CL_NAME) ": Source compiling..."
	@$(CC) $(CL_OBJ) -o $(CL_NAME)
	@echo "Executable "$(CL_NAME)" made"

$(SER_OBJ_DIR)%.o: $(SER_SRC_DIR)/%.c
	@$(CC) -c $< -o $@ $(CC_FLAGS) $(SER_HEADER_FLAGS)

$(CL_OBJ_DIR)%.o: $(CL_SRC_DIR)/%.c
	@$(CC) -c $< -o $@ $(CC_FLAGS) $(CL_HEADER_FLAGS)

clean:
	rm -f $(SER_OBJ) $(CL_OBJ)

fclean: clean
	rm -f $(SER_NAME) $(CL_NAME)

re: fclean all
	@echo "Make re done!"