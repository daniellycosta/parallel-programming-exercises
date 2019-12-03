#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int main(void)
{
  int n, i, j, k;
  printf("Enter the number of elements\n");
  scanf("%d", &n);

  int x[n];
  int prefix_sum[n];

  printf("Enter the vector x (%d elements)\n", n);
  for (int i = 0; i < n; i++)
  {
    scanf("%d", &x[i]);
    prefix_sum[i] = 0;
  }

  prefix_sum[0] = x[0];
  j = 1;
  for (i = 1; i < n; i *= 2)
  {
    prefix_sum[i] = prefix_sum[i - 1] + x[i];
    for (int k = 1; k <= j; k++)
    {
      prefix_sum[i + k] = x[i + k] + prefix_sum[i + k - 1];
    }
    j++;
  }

  printf("\nThe Answer: ");
  for (int i = 0; i < n; i++)
    printf("%d ", prefix_sum[i]);
  printf("\n");

  return 0;
}