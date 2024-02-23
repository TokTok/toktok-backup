FROM alpine:3.21.0

RUN ["apk", "add", "git", "py3-pip"]
RUN ["python3", "-m", "venv", "/path/to/venv"]
RUN . /path/to/venv/bin/activate \
 && pip install "git+https://github.com/josegonzalez/python-github-backup.git#egg=github-backup"

COPY entrypoint.sh /
ENTRYPOINT ["/entrypoint.sh"]
