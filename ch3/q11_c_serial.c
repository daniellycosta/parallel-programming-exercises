#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int main(void)
{
  int n, k, par;
  printf("Enter the number of elements\n");
  scanf("%d", &n);

  int x[n];
  int prefix_sum[n];

  printf("Enter the vector x (%d elements)\n", n);
  for (int i = 0; i < n; i++)
  {
    scanf("%d", &x[i]);
  }

  prefix_sum[0] = x[0];
  for (int i = 1; i <= n; i *= 2)
  {
    prefix_sum[i] = prefix_sum[i - i] + x[i];
    prefix_sum[i + 1] = prefix_sum[i] + x[i + 1];
  }

  printf("\nThe Answer: ");
  for (int i = 0; i < n; i++)
    printf("%d ", x[i]);
  printf("\n");

  return 0;
}