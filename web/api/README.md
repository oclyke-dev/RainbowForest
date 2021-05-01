# rainbow forest api
this code runs a server which maintains and distributes the state of the rainbow forest

## deployment
```
cd /web/api
docker build -t oclykedev/rainbow-forest-api .
docker push oclykedev/rainbow-forest-api
kubectl set image rainbow-forest-api main=oclykdev/rainbow-forest-api:latest
```

## secrets
the file ```src/secrets.ts``` must be created by the user to supply certain sensitive items (e.g. authorization credentials)

the format looks like this (however the secret value is of course incorrect):
``` javascript
export const truth = 'authorization-secret-value';

```
