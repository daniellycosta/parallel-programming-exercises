#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  int n;
  printf("Enter the value of n\n");
  scanf("%d", &n);

  int x[n];
  int prefix_sum[n];

  printf("Enter the vector x\n");
  for (int i = 0; i < n; i++)
    scanf("%d", &x[i]);

  prefix_sum[0] = x[0];
  printf("\nThe Answer: ");
  printf(" %d", prefix_sum[0]);

  for (int i = 1; i < n; i++)
  {
    printf(", ");
    prefix_sum[i] = prefix_sum[i - 1] + x[i];
    printf("%d ", prefix_sum[i]);
  }
  printf("\n");

  return 0;
}