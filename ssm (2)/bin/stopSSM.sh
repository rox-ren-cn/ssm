echo "SSM shutdown successfully!"
ps -ef | awk '/SSM/ && !/awk/{print $2}'|xargs kill -9
