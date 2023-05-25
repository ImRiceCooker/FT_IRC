FROM ubuntu:latest

RUN apt-get -y update \
	&& apt-get -y upgrade \
	&& apt-get install -y inspircd irssi tcpflow

COPY ./setup/irc_setup.sh tmp/

RUN chmod +x /tmp/irc_setup.sh
RUN echo "alias myserver='inspircd --runasroot --nofork'" >> ~/.bashrc
RUN echo "alias myclient='irssi -c localhost -p 6667'" >> ~/.bashrc
RUN echo "alias mytcp='tcpflow -i lo -c'" >> ~/.bashrc

ENTRYPOINT [ "tmp/irc_setup.sh" ]
