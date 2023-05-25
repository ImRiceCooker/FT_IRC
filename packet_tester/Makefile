all : init

init :
	docker build -t irc .

on :
	docker run --name irc irc

up :
	docker start irc

exec :
	docker exec -it irc /bin/bash
	
down :
	docker stop irc

clean : down
	docker rm irc

fclean :
	docker rmi $$(docker images -a)
	docker system purne -f
