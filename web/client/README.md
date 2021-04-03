
# rainbow forest client interface
this allows users to see the state of the rainbow forest from anywhere in the world and hear the songs composed by visitors to the Museum of Boulder

# updating
``` bash
docker build -t oclykedev/rainbow-forest-ui .
docker push oclykedev/rainbow-forest-ui
kubectl set image deployments/rainbow-forest-ui main=oclykedev/rainbow-forest-ui 
```
