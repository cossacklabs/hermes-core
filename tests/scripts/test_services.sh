#! /bin/bash

# testing with running services:
# 1. delete all files in key_store/data_store from previous tests
# 2. start services and save their PID in env variable to kill them at finish
# 3. run test
# 4. save test exit code in variable to not break test running and halting services correctly
# 5. stop services
# 6. return test exit code

mkdir -p db/key_store db/data_store db/credential_store
rm -rf db/key_store/*;
rm -rf db/data_store/*;

# wait when 8888 port (credential store) will be free
while nc localhost -z 8888 < /dev/null; do sleep 1; done

# wait when 8888 port (key store) will be free
while nc localhost -z 8889 < /dev/null; do sleep 1; done

# wait when 8888 port (data store) will be free
while nc localhost -z 8890 < /dev/null; do sleep 1; done

echo "start services"
./docs/examples/c/mid_hermes/credential_store_service/cs 1>/dev/null &
CS_PID=$!;
echo "credential store service pid=$CS_PID"

./docs/examples/c/mid_hermes/key_store_service/ks 1>/dev/null &
KS_PID=$!;
echo "key store service pid=$KS_PID"

./docs/examples/c/mid_hermes/data_store_service/ds 1>/dev/null &
DS_PID=$!;
echo "data store service pid=$DS_PID"

# run script from arg1
eval $@;
RESULT=$?;

echo "kill credential store $CS_PID";
kill -s SIGINT "$CS_PID";
echo "kill key store $KS_PID";
kill -s SIGINT "$KS_PID";
echo "kill data store $DS_PID";
kill -s SIGINT "$DS_PID";

# wait killing processes
echo "wait kill finishing"
while kill -0 "$CS_PID"; do sleep 0.5
done;
while kill -0 "$KS_PID"; do sleep 0.5
done;
while kill -0 "$DS_PID"; do sleep 0.5
done;

exit $RESULT
