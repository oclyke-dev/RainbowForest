# rainbow forest api
this code runs a server which maintains and distributes the state of the rainbow forest

## secrets
the file ```src/secrets.ts``` must be created by the user to supply certain sensitive items (e.g. authorization credentials)

the format looks like this (however the secret value is of course incorrect):
``` javascript
export const truth = 'authorization-secret-value';

```
